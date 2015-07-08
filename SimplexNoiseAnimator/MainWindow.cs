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

using SimplexNoise;

namespace SimplexNoiseAnimator
{
    public partial class MainWindow : Form
    {
        private List<double> m_sinus = new List<double>();
        private List<double> m_cosinus = new List<double>();
        private List<double> m_triangle = new List<double>();

        public MainWindow()
        {
            InitializeComponent();

            updateWaveTables();
            updatePreview();

            this.StartPosition = FormStartPosition.CenterScreen;
        }

        /// <summary>
        /// clamps NumericUpDown values to their specified range
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void numericUpDown_ValueChanged(object sender, EventArgs e)
        {
            var control = (NumericUpDown)sender;
            if(control.Value < control.Minimum)
            {
                control.Value = control.Minimum;
            }
            else if(control.Value > control.Maximum)
            {
                control.Value = control.Maximum;
            }

            if(numericUpDownCurrentFrame.Value >= numericUpDownFrameCount.Value)
            {
                numericUpDownCurrentFrame.Value = numericUpDownFrameCount.Value - 1;
            }

            updateWaveTables();
            updatePreview();
        }

        private void numericUpDownCurrentFrame_ValueChanged(object sender, EventArgs e)
        {
            var control = (NumericUpDown)sender;
            if (control.Value < control.Minimum)
            {
                control.Value = control.Minimum;
            }
            else if (control.Value >= numericUpDownFrameCount.Value)
            {
                control.Value = numericUpDownFrameCount.Value - 1;
            }

            updatePreview();
        }

        /// <summary>
        /// opens a colour picker for the clicked control
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void panelColour_Click(object sender, EventArgs e)
        {
            var control = (Panel)sender;
            ColorDialog cd = new ColorDialog();
            cd.Color = control.BackColor;
            cd.AllowFullOpen = true;
            cd.AnyColor = true;
            cd.FullOpen = true;

            if(cd.ShowDialog() == DialogResult.OK)
            {
                control.BackColor = cd.Color;
                updatePreview();
            }
        }

        /// <summary>
        /// generates map based on current settings
        /// </summary>
        private void updatePreview()
        {
            var colour1 = panelStartColour.BackColor;
            var colour2 = panelEndColour.BackColor;

            var index = (int)numericUpDownCurrentFrame.Value;
            float xOffset = (float)m_sinus[index];
            float yOffset = (float)m_cosinus[index];
            float z = (float)m_triangle[index];

            Bitmap bitmap = new Bitmap((int)numericUpDownWidth.Value, (int)numericUpDownHeight.Value);

            float divisorX = 100f * (float)numericUpDownScaleX.Value;
            float divisorY = 100f * (float)numericUpDownScaleY.Value;
            float divisorZ = (divisorX + divisorY) / 2f;

            for(var y = 0; y < (int)numericUpDownHeight.Value; ++y)
            {
                for(var x = 0; x < (int)numericUpDownWidth.Value; ++x)
                {
                    var noiseVal = (Noise.Generate(((float)x + xOffset) / divisorX, ((float)y + yOffset) / divisorY) + 1f) / 2f;
                    bitmap.SetPixel(x, y, lerp(colour1, colour2, noiseVal));
                }
            }

            panelPreview.BackgroundImage = bitmap;
            panelPreview.BackgroundImageLayout = ImageLayout.Center;
        }

        /// <summary>
        /// updates the wavetables based on current values
        /// </summary>
        private void updateWaveTables()
        {
            m_sinus.Clear();
            m_cosinus.Clear();
            m_triangle.Clear();

            double theta = (Math.PI * 2f) / (double)numericUpDownFrameCount.Value;
            double radius = (double)numericUpDownWidth.Value * 5;

            double depthRadius = radius * ((double)numericUpDownFrameCount.Value / 100);
            double step = depthRadius / (double)numericUpDownFrameCount.Value;

            for(int i = 0; i < (int)numericUpDownFrameCount.Value; ++i)
            {
                double angle = theta * i;
                m_sinus.Add(((Math.Sin(angle) + 1) / 2) * radius);
                m_cosinus.Add(((Math.Cos(angle) + 1) / 2) * radius);

                double depth = step * i;
                if (depth < depthRadius / 2)
                    m_triangle.Add(depth);
                else 
                    m_triangle.Add(depthRadius - depth);
            }
        }


        /// <summary>
        /// interpolates two colours
        /// </summary>
        /// <param name="c1"></param>
        /// <param name="c2"></param>
        /// <param name="t"></param>
        /// <returns></returns>
        private Color lerp(Color c1, Color c2, float t)
        {
            //t = Math.Min(Math.Max(0f, t), 1f);
            return Color.FromArgb(lerp(c1.R, c2.R, t), lerp(c1.G, c2.G, t), lerp(c1.B, c2.B, t));
        }

        private int lerp(int a, int b, float t)
        {
            float tp = 1.0f - t;
            return (int)(tp * a + t * b);
        }

        private void buttonRender_Click(object sender, EventArgs e)
        {
            SaveFileDialog sd = new SaveFileDialog();
            sd.Filter = "Portable Network Graphic|*.png";
            if(sd.ShowDialog() == DialogResult.OK)
            {
                for(int i = 0; i < numericUpDownFrameCount.Value; ++i)
                {
                    numericUpDownCurrentFrame.Value = i;
                    panelPreview.BackgroundImage.Save(sd.FileName.Replace(".png", "00" + i.ToString() + ".png"));
                }
            }
        }
    }
}
