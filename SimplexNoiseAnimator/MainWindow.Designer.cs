namespace SimplexNoiseAnimator
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.panelPreview = new System.Windows.Forms.Panel();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.buttonSave = new System.Windows.Forms.Button();
            this.numericUpDownWidth = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownHeight = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownFrameCount = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownCurrentFrame = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.panelStartColour = new System.Windows.Forms.Panel();
            this.panelEndColour = new System.Windows.Forms.Panel();
            this.buttonRender = new System.Windows.Forms.Button();
            this.buttonOpen = new System.Windows.Forms.Button();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.numericUpDownScaleX = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownScaleY = new System.Windows.Forms.NumericUpDown();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownWidth)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownHeight)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownFrameCount)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownCurrentFrame)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownScaleX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownScaleY)).BeginInit();
            this.SuspendLayout();
            // 
            // panelPreview
            // 
            this.panelPreview.BackColor = System.Drawing.Color.Black;
            this.panelPreview.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panelPreview.Location = new System.Drawing.Point(12, 12);
            this.panelPreview.Name = "panelPreview";
            this.panelPreview.Size = new System.Drawing.Size(256, 256);
            this.panelPreview.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(274, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(38, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Width:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(274, 38);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(41, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Height:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 286);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(76, 13);
            this.label3.TabIndex = 11;
            this.label3.Text = "Current Frame:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(274, 64);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(70, 13);
            this.label4.TabIndex = 5;
            this.label4.Text = "Frame Count:";
            // 
            // buttonSave
            // 
            this.buttonSave.Location = new System.Drawing.Point(237, 281);
            this.buttonSave.Name = "buttonSave";
            this.buttonSave.Size = new System.Drawing.Size(75, 23);
            this.buttonSave.TabIndex = 14;
            this.buttonSave.Text = "Save";
            this.buttonSave.UseVisualStyleBackColor = true;
            // 
            // numericUpDownWidth
            // 
            this.numericUpDownWidth.Location = new System.Drawing.Point(350, 10);
            this.numericUpDownWidth.Maximum = new decimal(new int[] {
            256,
            0,
            0,
            0});
            this.numericUpDownWidth.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.numericUpDownWidth.Name = "numericUpDownWidth";
            this.numericUpDownWidth.Size = new System.Drawing.Size(46, 20);
            this.numericUpDownWidth.TabIndex = 2;
            this.numericUpDownWidth.Value = new decimal(new int[] {
            256,
            0,
            0,
            0});
            this.numericUpDownWidth.ValueChanged += new System.EventHandler(this.numericUpDown_ValueChanged);
            // 
            // numericUpDownHeight
            // 
            this.numericUpDownHeight.Location = new System.Drawing.Point(350, 36);
            this.numericUpDownHeight.Maximum = new decimal(new int[] {
            256,
            0,
            0,
            0});
            this.numericUpDownHeight.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.numericUpDownHeight.Name = "numericUpDownHeight";
            this.numericUpDownHeight.Size = new System.Drawing.Size(46, 20);
            this.numericUpDownHeight.TabIndex = 4;
            this.numericUpDownHeight.Value = new decimal(new int[] {
            256,
            0,
            0,
            0});
            this.numericUpDownHeight.ValueChanged += new System.EventHandler(this.numericUpDown_ValueChanged);
            // 
            // numericUpDownFrameCount
            // 
            this.numericUpDownFrameCount.Location = new System.Drawing.Point(350, 62);
            this.numericUpDownFrameCount.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownFrameCount.Name = "numericUpDownFrameCount";
            this.numericUpDownFrameCount.Size = new System.Drawing.Size(46, 20);
            this.numericUpDownFrameCount.TabIndex = 6;
            this.numericUpDownFrameCount.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownFrameCount.ValueChanged += new System.EventHandler(this.numericUpDown_ValueChanged);
            // 
            // numericUpDownCurrentFrame
            // 
            this.numericUpDownCurrentFrame.Location = new System.Drawing.Point(94, 284);
            this.numericUpDownCurrentFrame.Maximum = new decimal(new int[] {
            99,
            0,
            0,
            0});
            this.numericUpDownCurrentFrame.Name = "numericUpDownCurrentFrame";
            this.numericUpDownCurrentFrame.ReadOnly = true;
            this.numericUpDownCurrentFrame.Size = new System.Drawing.Size(40, 20);
            this.numericUpDownCurrentFrame.TabIndex = 12;
            this.numericUpDownCurrentFrame.ValueChanged += new System.EventHandler(this.numericUpDownCurrentFrame_ValueChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(274, 218);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(32, 13);
            this.label5.TabIndex = 7;
            this.label5.Text = "Start:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(338, 218);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(29, 13);
            this.label6.TabIndex = 9;
            this.label6.Text = "End:";
            // 
            // panelStartColour
            // 
            this.panelStartColour.BackColor = System.Drawing.Color.Black;
            this.panelStartColour.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panelStartColour.Location = new System.Drawing.Point(274, 234);
            this.panelStartColour.Name = "panelStartColour";
            this.panelStartColour.Size = new System.Drawing.Size(54, 33);
            this.panelStartColour.TabIndex = 8;
            this.panelStartColour.Click += new System.EventHandler(this.panelColour_Click);
            // 
            // panelEndColour
            // 
            this.panelEndColour.BackColor = System.Drawing.Color.White;
            this.panelEndColour.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panelEndColour.Location = new System.Drawing.Point(341, 234);
            this.panelEndColour.Name = "panelEndColour";
            this.panelEndColour.Size = new System.Drawing.Size(54, 33);
            this.panelEndColour.TabIndex = 10;
            this.panelEndColour.Click += new System.EventHandler(this.panelColour_Click);
            // 
            // buttonRender
            // 
            this.buttonRender.Location = new System.Drawing.Point(320, 281);
            this.buttonRender.Name = "buttonRender";
            this.buttonRender.Size = new System.Drawing.Size(75, 23);
            this.buttonRender.TabIndex = 15;
            this.buttonRender.Text = "Render";
            this.buttonRender.UseVisualStyleBackColor = true;
            this.buttonRender.Click += new System.EventHandler(this.buttonRender_Click);
            // 
            // buttonOpen
            // 
            this.buttonOpen.Location = new System.Drawing.Point(156, 281);
            this.buttonOpen.Name = "buttonOpen";
            this.buttonOpen.Size = new System.Drawing.Size(75, 23);
            this.buttonOpen.TabIndex = 13;
            this.buttonOpen.Text = "Open";
            this.buttonOpen.UseVisualStyleBackColor = true;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(274, 108);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(47, 13);
            this.label7.TabIndex = 16;
            this.label7.Text = "Scale X:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(274, 134);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(47, 13);
            this.label8.TabIndex = 17;
            this.label8.Text = "Scale Y:";
            // 
            // numericUpDownScaleX
            // 
            this.numericUpDownScaleX.DecimalPlaces = 1;
            this.numericUpDownScaleX.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.numericUpDownScaleX.Location = new System.Drawing.Point(350, 106);
            this.numericUpDownScaleX.Maximum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.numericUpDownScaleX.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.numericUpDownScaleX.Name = "numericUpDownScaleX";
            this.numericUpDownScaleX.Size = new System.Drawing.Size(45, 20);
            this.numericUpDownScaleX.TabIndex = 18;
            this.numericUpDownScaleX.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownScaleX.ValueChanged += new System.EventHandler(this.numericUpDown_ValueChanged);
            // 
            // numericUpDownScaleY
            // 
            this.numericUpDownScaleY.DecimalPlaces = 1;
            this.numericUpDownScaleY.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.numericUpDownScaleY.Location = new System.Drawing.Point(350, 132);
            this.numericUpDownScaleY.Maximum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.numericUpDownScaleY.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.numericUpDownScaleY.Name = "numericUpDownScaleY";
            this.numericUpDownScaleY.Size = new System.Drawing.Size(46, 20);
            this.numericUpDownScaleY.TabIndex = 19;
            this.numericUpDownScaleY.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDownScaleY.ValueChanged += new System.EventHandler(this.numericUpDown_ValueChanged);
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(407, 320);
            this.Controls.Add(this.numericUpDownScaleY);
            this.Controls.Add(this.numericUpDownScaleX);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.buttonOpen);
            this.Controls.Add(this.buttonRender);
            this.Controls.Add(this.panelEndColour);
            this.Controls.Add(this.panelStartColour);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.numericUpDownCurrentFrame);
            this.Controls.Add(this.numericUpDownFrameCount);
            this.Controls.Add(this.numericUpDownHeight);
            this.Controls.Add(this.numericUpDownWidth);
            this.Controls.Add(this.buttonSave);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.panelPreview);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(423, 355);
            this.MinimumSize = new System.Drawing.Size(423, 355);
            this.Name = "MainWindow";
            this.Text = "Animated Noise";
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownWidth)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownHeight)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownFrameCount)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownCurrentFrame)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownScaleX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownScaleY)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel panelPreview;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button buttonSave;
        private System.Windows.Forms.NumericUpDown numericUpDownWidth;
        private System.Windows.Forms.NumericUpDown numericUpDownHeight;
        private System.Windows.Forms.NumericUpDown numericUpDownFrameCount;
        private System.Windows.Forms.NumericUpDown numericUpDownCurrentFrame;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Panel panelStartColour;
        private System.Windows.Forms.Panel panelEndColour;
        private System.Windows.Forms.Button buttonRender;
        private System.Windows.Forms.Button buttonOpen;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.NumericUpDown numericUpDownScaleX;
        private System.Windows.Forms.NumericUpDown numericUpDownScaleY;
    }
}

