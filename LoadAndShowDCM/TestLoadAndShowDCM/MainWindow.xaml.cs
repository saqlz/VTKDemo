using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace TestLoadAndShowDCM
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            new Thread(() =>
            {
                 new SocketService();
            }).Start();
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            var width = Convert.ToInt32(TestLoadAndShowCanvas.ActualWidth);
            var height = Convert.ToInt32(TestLoadAndShowCanvas.ActualHeight);
            var path = this.ITextBoxForPath.Text;
            int flag = NativeMethods.LoadAndShowByPathAndDim(new StringBuilder(path), 145, 146);
            if (0 == flag)
            {
                Thread.Sleep(500);
                var bitmap = BitmapHelpers.GeneratedBitmapByBitmapImageData(SocketService.ImageArray, width, height);
                var bitmapImage = BitmapHelpers.ConvertByteArrayToBitmapImage(BitmapHelpers.ConvertBitmapToByteArray(bitmap));
                var image = new Image() { Source = bitmapImage };
                this.TestLoadAndShowCanvas.Children.Add(image);
            }
            else
            {
                MessageBox.Show("Fail to create Filename", "xx");
            }
        }
    }
}
