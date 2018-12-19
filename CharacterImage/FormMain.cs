using CharacterImageX.Engines;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CharacterImageX
{
    public partial class FormMain : Form
    {
        Bitmap Image { get; set; } = null;

        List<char> Chars { get; } = new List<char>();

        public FormMain()
        {
            InitializeComponent();
        }

        private void LoadChars()
        {
            for (char c = 'A'; c <= 'Z'; c++) Chars.Add(c);
            for (char c = 'a'; c <= 'z'; c++) Chars.Add(c);
            Chars.Add(' ');
        }

        private void LoadFonts()
        {
            FontFamily[] fonts = FontFamily.Families;
            foreach (var font in fonts)
            {
                comboBox1.Items.Add(font.Name);
                int a = font.GetLineSpacing(FontStyle.Regular);
            }
            comboBox1.SelectedItem = FontFamily.GenericMonospace.Name;
        }

        private void FormMain_Load(object sender, EventArgs e)
        {
            LoadChars();
            LoadFonts();
            UpdateLabel();
        }

        private void UpdateLabel()
        {
            labelThreshold.Text = (trackBar1.Value).ToString();
            labelResize.Text = (trackBar2.Value / 10.0).ToString();
        }

        private void UpdateResult()
        {
            labelThreshold.Text = trackBar1.Value.ToString();
            if (Image != null)
            {
                FontFamily font = new FontFamily(comboBox1.SelectedItem as string);
                int lineSpacing = font.GetLineSpacing(FontStyle.Regular);
                int emHeight = font.GetEmHeight(FontStyle.Regular);

                Engine engine = new CppEngine(Chars, new Font(comboBox1.SelectedItem as string, (int)numericSize.Value));
                Bitmap b1 = PreProcessor.Threshold(Image, trackBar1.Value);
                b1 = PreProcessor.Resize(b1, trackBar2.Value / 10.0);

                SizeF size = CreateGraphics().MeasureString(Chars.First().ToString(), new Font(font, (float)numericSize.Value));
                int spacing = Convert.ToInt32(Math.Round(size.Width) / 2);
                int lineHeight = (int)(numericSize.Value * lineSpacing / emHeight);
                List<string> strings = engine.GetStrings(b1, spacing, lineHeight);
                Bitmap b2 = engine.DrawStrings(new Bitmap(b1.Width, b1.Height), strings, spacing, lineHeight);
                b2.Tag = strings;

                pictureBox1.Image = b1;
                pictureBox2.Image = b2;

                GC.Collect();
            }
        }

        private void trackBar1_MouseUp(object sender, MouseEventArgs e)
        {
            UpdateLabel();
            UpdateResult();
        }

        private void trackBar2_MouseUp(object sender, MouseEventArgs e)
        {
            UpdateLabel();
            UpdateResult();
        }

        private void trackBar1_KeyUp(object sender, KeyEventArgs e)
        {
            UpdateLabel();
            UpdateResult();
        }

        private void trackBar2_KeyUp(object sender, KeyEventArgs e)
        {
            UpdateLabel();
            UpdateResult();
        }

        private void numericSize_ValueChanged(object sender, EventArgs e)
        {
            UpdateResult();
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateResult();
        }

        private void pictureBox1_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                contextOpen.Show(pictureBox1, e.X, e.Y);
            }
        }

        private void pictureBox2_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                if (pictureBox2.Image != null) contextSave.Show(pictureBox2, e.X, e.Y);
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Images|*.jpg;*.png;*.bmp";

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    Image = new Bitmap(ofd.FileName);
                    trackBar1.Value = 0;
                    trackBar2.Value = 10;

                    UpdateLabel();
                    UpdateResult();
                }
                catch
                {

                }
            }
        }

        private void saveImageAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Image|*.jpg;*.png;*.bmp";

            if (sfd.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    pictureBox2.Image.Save(sfd.FileName);
                    MessageBox.Show("Success!");
                }
                catch
                {

                }
            }
        }

        private void saveStringsAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "Text|*.txt";

            if (sfd.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    List<string> lines = pictureBox2.Image.Tag as List<string>;
                    using (StreamWriter sw = new StreamWriter(sfd.FileName))
                    {
                        foreach (string line in lines)
                        {
                            sw.WriteLine(line);
                        }
                    }
                    MessageBox.Show("Success!");
                }
                catch
                {

                }
            }
        }

        private void copyStringsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Clipboard.SetText(string.Join(Environment.NewLine, pictureBox2.Image.Tag as List<string>));
            MessageBox.Show("Success!");
        }
    }
}
