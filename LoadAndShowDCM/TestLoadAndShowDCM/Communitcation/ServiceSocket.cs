

using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace TestLoadAndShowDCM
{
    public sealed class ServiceSocket
    {
        public void StartListen()
        {
            try
            {
                Socket receiveSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                IPEndPoint hostIpEndPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 20000);
                //设置接收数据缓冲区的大小 
                byte[] b = new byte[4096];
                receiveSocket.Bind(hostIpEndPoint);
                //监听
                receiveSocket.Listen(2);
                Console.WriteLine("已经开始监听");
                Socket hostSocket = receiveSocket.Accept();
                //如何确定该数组大小 
                MemoryStream fs = new MemoryStream();
                int length = 0;
                while ((length = hostSocket.Receive(b)) > 0)
                {
                    fs.Write(b, 0, length);
                }
                fs.Flush();
                Image bm = Image.FromStream(fs,true, true);
                bm.Save(AppDomain.CurrentDomain.BaseDirectory  + @"flash01.png", ImageFormat.Png);

                //关闭写文件流
                fs.Close();
                //关闭接收数据的Socket 
                hostSocket.Shutdown(SocketShutdown.Receive);
                hostSocket.Close();
                //关闭发送连接 
                receiveSocket.Close();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }
}
