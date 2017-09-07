using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using PixelFormat = System.Drawing.Imaging.PixelFormat;

namespace TestLoadAndShowDCM
{
    public static class BitmapHelpers
    {
        /// <summary>
        /// 使用BitmapData Class来将文件中读取为byte[]后生成Bitmap
        /// 难点是byte[]不能直接生成Bitmap，因为Bitmap存在信息头
        /// </summary>
        public static Bitmap GeneratedBitmapByBitmapImageData(byte[] imageArray, int imgWidth, int imgHeight,
            System.Drawing.Imaging.PixelFormat format = PixelFormat.Format24bppRgb)
        {
            var bitmap = new Bitmap(imgWidth, imgHeight, format);

            //Using BitmapData
            var bitmapImageData = bitmap.LockBits(new Rectangle(0, 0, bitmap.Width, bitmap.Height),
                ImageLockMode.ReadWrite, bitmap.PixelFormat);
            var channel = 3;
            unsafe
            {
                for (var heightIndex = 0; heightIndex < bitmap.Height; heightIndex++)
                {
                    byte* rowPointer = (byte*)bitmapImageData.Scan0 + heightIndex*bitmapImageData.Stride;
                    for (var widthIndex = 0; widthIndex < bitmap.Width; widthIndex++)
                    {
                        var offset = (heightIndex*bitmap.Width + widthIndex) * channel;
                        rowPointer[widthIndex*channel + 0] = imageArray[offset + 2];     //Blue
                        rowPointer[widthIndex * channel + 1] = imageArray[offset + 1] ;  //Green
                        rowPointer[widthIndex * channel + 2] = imageArray[offset + 0];   //Red
                    }
                }
            }
            bitmap.UnlockBits(bitmapImageData);
            return bitmap;
        }

        public static Bitmap GetBitmapByAppendHeader(byte[] imageArray, int imgWidth, int imgHeight)
        {
            //https://stackoverflow.com/questions/11452246/add-a-bitmap-header-to-a-byte-array-then-create-a-bitmap-file
            // bmpBufferSize : a pure length of raw bitmap data without the header.
            int bmpBufferSize = imageArray.Length;
            
            // the 54 value here is the length of bitmap header.
            byte[] bitmapBytes = new byte[bmpBufferSize + 54];

            #region Bitmap Header 位图文件头（bitmap-file header）

            // 0~2 "BM" WIndows系统下默认值
            bitmapBytes[0] = 0x42;
            bitmapBytes[1] = 0x4d;

            // 2~6 Size of the BMP file - Bit cound + Header 54 整个文件的大小
            Array.Copy(BitConverter.GetBytes(bmpBufferSize + 54), 0, bitmapBytes, 2, 4);

            // 6~8 Application Specific : normally, set zero 一般为0
            Array.Copy(BitConverter.GetBytes(0), 0, bitmapBytes, 6, 2);

            // 8~10 Application Specific : normally, set zero  一般为0
            Array.Copy(BitConverter.GetBytes(0), 0, bitmapBytes, 8, 2);

            // 10~14 Offset where the pixel array can be found - 24bit bitmap data always starts at 54 offset.
            Array.Copy(BitConverter.GetBytes(54), 0, bitmapBytes, 10, 4);
            #endregion

            #region DIB Header 位图信息头（bitmap-information header）
            // 14~18 Number of bytes in the DIB header. 40 bytes constant.
            Array.Copy(BitConverter.GetBytes(40), 0, bitmapBytes, 14, 4);

            // 18~22 Width of the bitmap.
            Array.Copy(BitConverter.GetBytes(imgWidth), 0, bitmapBytes, 18, 4);

            // 22~26 Height of the bitmap.
            Array.Copy(BitConverter.GetBytes(imgHeight), 0, bitmapBytes, 22, 4);

            // 26~28 Number of color planes being used
            Array.Copy(BitConverter.GetBytes(0), 0, bitmapBytes, 26, 2);

            // 28~30 Number of bits. If you don't know the pixel format, trying to calculate it with the quality of the video/image source.
            //if (image.PixelFormat == System.Drawing.Imaging.PixelFormat.Format24bppRgb)
            {
                Array.Copy(BitConverter.GetBytes(24), 0, bitmapBytes, 28, 2);
            }

            // 30~34 BI_RGB no pixel array compression used : most of the time, just set zero if it is raw data.
            Array.Copy(BitConverter.GetBytes(0), 0, bitmapBytes, 30, 4);

            // 34~38 Size of the raw bitmap data ( including padding )
            Array.Copy(BitConverter.GetBytes(bmpBufferSize), 0, bitmapBytes, 34, 4);

            // 38~46 Print resolution of the image, 72 DPI x 39.3701 inches per meter yields
            // if (image.PixelFormat == System.Drawing.Imaging.PixelFormat.Format24bppRgb)
            {
                Array.Copy(BitConverter.GetBytes(0), 0, bitmapBytes, 38, 4);
                Array.Copy(BitConverter.GetBytes(0), 0, bitmapBytes, 42, 4);
            }

            // 46~50 Number of colors in the palette
            Array.Copy(BitConverter.GetBytes(0), 0, bitmapBytes, 46, 4);

            // 50~54 means all colors are important
            Array.Copy(BitConverter.GetBytes(0), 0, bitmapBytes, 50, 4);

            // 54~end : Pixel Data : Finally, time to combine your raw data, BmpBuffer in this code, with a bitmap header you've just created.
            Array.Copy(imageArray as Array, 0, bitmapBytes, 54, bmpBufferSize);
            #endregion
            
            using (var ms = new MemoryStream(bitmapBytes))
            {
                return new Bitmap((Image)new Bitmap(ms));
            }
        }

        /// <summary>
        /// 将Bitmap转换成BitmapImage
        /// </summary>
        public static BitmapImage ConvertBitmapToBitmapImage(Bitmap bitmap)
        {
            return ConvertByteArrayToBitmapImage(ConvertBitmapToByteArray(bitmap));
        }

        /// <summary>
        /// 将Bitmap转换成byte数组
        /// </summary>
        public static byte[] ConvertBitmapToByteArray(Bitmap bitmap)
        {
            try
            {
                using (var ms = new MemoryStream())
                {
                    bitmap.Save(ms, bitmap.RawFormat);
                    return ms.ToArray();
                }
            }
            catch(Exception)
            {
                using (var ms = new MemoryStream())
                {
                    bitmap.Save(ms, ImageFormat.Bmp);
                    return ms.ToArray();
                }
            }
        }

        /// <summary>
        /// 将Bitmap转换成byte数组后继续转换成BitmapImage
        /// </summary>
        public static BitmapImage ConvertByteArrayToBitmapImage(byte[] imageArray)
        {
            using (var ms = new MemoryStream(imageArray))
            {
                var image = new BitmapImage();
                image.BeginInit();
                image.CacheOption = BitmapCacheOption.OnLoad; // here
                image.StreamSource = ms;
                image.EndInit();
                return image;
            }
        }

        /// <summary>
        /// 将Bitmap转换成BitmapImage
        /// </summary>
        [Obsolete]
        public static BitmapImage ConvertBitmapToBitmapImage1(Bitmap bitmap)
        {
            var bitmapImage = new BitmapImage();
            using (var ms = new MemoryStream())
            {
                bitmap.Save(ms, ImageFormat.Png);
                bitmapImage.BeginInit();
                bitmapImage.StreamSource = ms;
                bitmapImage.CacheOption = BitmapCacheOption.OnLoad;
                bitmapImage.EndInit();
                bitmapImage.Freeze();
            }
            return bitmapImage;
        }
        
        ///// <summary>
        ///// Test 
        ///// </summary>
        //public static ImageFormat GetImageFormat(Bitmap bitmap)
        //{
        //    var ms = new MemoryStream();
        //    foreach (var format in new ImageFormat[]{
        //                            ImageFormat.Bmp,
        //                            ImageFormat.Emf,
        //                            ImageFormat.Exif,
        //                            ImageFormat.Gif,
        //                            ImageFormat.Icon,
        //                            ImageFormat.Jpeg,
        //                            ImageFormat.MemoryBmp,
        //                            ImageFormat.Png,
        //                            ImageFormat.Tiff,
        //                            ImageFormat.Wmf})
        //    {

        //        try
        //        {
        //            bitmap.Save(ms, format);
        //            return format;
        //        }
        //        catch (Exception) { }
        //    }
        //    return ImageFormat.Bmp;
        //}

    }
}
