using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO.Ports;
using System.IO;

namespace Motors
{
    public partial class Form1 : Form
    {
        SerialPort port = new SerialPort("COM1", 9600);
        Thread th;
        byte[] Command = new byte[2];
        byte[] s;
        string text = "";
        bool M1F = false;
        bool M2F = false;
        bool M1B = false;
        bool M2B = false;
        bool M1S = true;
        bool M2S = true;

        public Form1()
        {
            InitializeComponent();
            port.Open();
            th = new Thread(Scan);
            th.Start();
        }

        private void Scan()
        {
            while (true)
            {
                s = new byte[port.BytesToRead];
                port.Read(s, 0, port.BytesToRead);
                text += s;
                s = new byte[port.BytesToRead];
                port.Read(s, 0, port.BytesToRead);
                text += "\t" + s + "\r\n";
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            M1F = true; M1B = false; M1S = false;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            M1F = false; M1B = true; M1S = false;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            M2F = true; M2B = false; M2S = false;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            M2F = false; M2B = true; M2S = false;
        }

        private void button5_Click(object sender, EventArgs e)
        {
            Command[0] = Command[1] = 0;

            if (!M1S && M2S)
            {
                if (M1F)
                {
                    textBox3.Text = "1 Мотор вперед";
                    Command[1] |= 0x10;
                    Command[0] |= Convert.ToByte(textBox1.Text);
                }
                if (M1B)
                {
                    textBox3.Text = "1 Мотор назад";
                    Command[1] |= 0x30;
                    Command[0] |= Convert.ToByte(textBox1.Text);
                }
            }

            if (M1S && M2S)
            {
                    textBox3.Text = "Стоп";             
            }
            if (M1S && !M2S)
            {
                if (M2F)
                {
                    textBox3.Text = "2 Мотор вперед";
                    Command[1] |= 0x20;
                    Command[0] |= Convert.ToByte(textBox1.Text);
                }
                if (M2B)
                {
                    textBox3.Text = "2 Мотор назад";
                    Command[1] |= 0x40;
                    Command[0] |= Convert.ToByte(textBox1.Text);
                }
            }
            if (!M1S && !M2S)
            {
                if (M1F && M2F)
                {
                    textBox3.Text = "Оба мотора вперед";
                    Command[1] |= 0x50;
                    Command[0] |= Convert.ToByte(textBox1.Text);
                }
                if (M1B && M2B)
                {
                    textBox3.Text = "Оба мотора назад";
                    Command[1] |= 0x60;
                    Command[0] |= Convert.ToByte(textBox1.Text);
                }
                if (M1F && M2B)
                {
                    textBox3.Text = "1 Мотор вперед, 2 мотор назад";
                    Command[1] |= 0x70;
                    Command[0] |= Convert.ToByte(textBox1.Text);
                }
                if (M2F && M1B)
                {
                    textBox3.Text = "2 Мотор вперед, 1 мотор назад";
                    Command[1] |= 0x80;
                    Command[0] |= Convert.ToByte(textBox1.Text);
                }
            }
            port.Write(Command, 0, 1);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            StreamWriter sw;
            FileInfo fi = new FileInfo("Encoders.txt");
            sw = fi.AppendText();
            sw.Write(text);
            sw.Close();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            th.Abort();
        }

        private void button7_Click(object sender, EventArgs e)
        {
            M1F = false;
            //M2F = false;
            M1B = false;
            //M2B = false;
            M1S = true;
            //M2S = true;
        }

        private void button8_Click(object sender, EventArgs e)
        {
            //M1F = false;
            M2F = false;
            //M1B = false;
            M2B = false;
            //M1S = true;
            M2S = true;
        }
    }
}
