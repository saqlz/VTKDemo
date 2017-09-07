using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace TestLoadAndShowDCM
{
    public class SocketService
    {
        private const string HostName = "127.0.0.1";
        private const int HostPort = 20000;
        private readonly Socket _listenSocket = null;
        public static byte[] ImageArray { get; private set; }

        public SocketService()
        {
            _listenSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            _listenSocket.Bind(new IPEndPoint(IPAddress.Parse(HostName), HostPort));
            _listenSocket.Listen(0);
            while (true)
            {
                var acceptSocket = _listenSocket.Accept();
                ImageArray = Receive(acceptSocket, 5000); //5 seconds timeout.
            }
        }

        ~SocketService()
        {
             if (_listenSocket != null && _listenSocket.Connected)
             {
                _listenSocket.Shutdown(SocketShutdown.Both);
                _listenSocket.Close();
             }
        }

        private static byte[] Receive(Socket socket, int timeout)
        {
            socket.ReceiveTimeout = timeout;
            List<byte> data = new List<byte>();
            byte[] buffer = new byte[1024];
            var length = 0;
            try
            {
                while ((length = socket.Receive(buffer)) > 0)
                {
                    for (int j = 0; j < length; j++)
                    {
                        data.Add(buffer[j]);
                    }
                    if (length < buffer.Length)
                    {
                        break;
                    }
                }
            }
            catch(Exception)
            {
                
            }
            return data.ToArray();
        }

    }
}
