using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            var width = Convert.ToInt32(TestLoadAndShowCanvas.ActualWidth);
            var height = Convert.ToInt32(TestLoadAndShowCanvas.ActualHeight);
            int flag = NativeMethods.LoadAndShowByPathAndDim(this.ITextBoxForPath.Text, width, height);
            if (0 == flag)
            {
                var bitmap = new BitmapImage(new Uri(@"D:\MyWorks\VTKDemo\LoadAndShowDCM\LoadAndShowDCM\" + @"TestDelaunay2D.png",
                                      UriKind.Absolute));
                var image = new Image();
                image.Source = bitmap;
                this.TestLoadAndShowCanvas.Children.Add(image);
            }
            else
            {
                MessageBox.Show("Fail to create Filename", "xx");
            }
        }
    }
}
