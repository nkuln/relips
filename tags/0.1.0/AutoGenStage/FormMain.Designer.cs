namespace AutoGenStage
{
    partial class FormMain
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
            this.components = new System.ComponentModel.Container();
            this.uxBrowseMusic = new System.Windows.Forms.Button();
            this.uxMusicFilePath = new System.Windows.Forms.TextBox();
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.uxGenerate = new System.Windows.Forms.Button();
            this.uxChooseAudioGroup = new System.Windows.Forms.GroupBox();
            this.uxOutputLabel = new System.Windows.Forms.Label();
            this.uxMusicLabel = new System.Windows.Forms.Label();
            this.uxOutputFilePath = new System.Windows.Forms.TextBox();
            this.uxBrowseOutput = new System.Windows.Forms.Button();
            this.uxMsg = new System.Windows.Forms.Label();
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.uxProgress = new System.Windows.Forms.ToolStripProgressBar();
            this.saveFileDialog = new System.Windows.Forms.SaveFileDialog();
            this.errorProvider = new System.Windows.Forms.ErrorProvider(this.components);
            this.uxChooseAudioGroup.SuspendLayout();
            this.statusStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider)).BeginInit();
            this.SuspendLayout();
            // 
            // uxBrowseMusic
            // 
            this.uxBrowseMusic.Location = new System.Drawing.Point(359, 22);
            this.uxBrowseMusic.Margin = new System.Windows.Forms.Padding(4);
            this.uxBrowseMusic.Name = "uxBrowseMusic";
            this.uxBrowseMusic.Size = new System.Drawing.Size(100, 28);
            this.uxBrowseMusic.TabIndex = 0;
            this.uxBrowseMusic.Text = "Browse ..";
            this.uxBrowseMusic.UseVisualStyleBackColor = true;
            this.uxBrowseMusic.Click += new System.EventHandler(this.uxBrowseMusic_Click);
            // 
            // uxMusicFilePath
            // 
            this.uxMusicFilePath.Location = new System.Drawing.Point(100, 25);
            this.uxMusicFilePath.Margin = new System.Windows.Forms.Padding(4);
            this.uxMusicFilePath.Name = "uxMusicFilePath";
            this.uxMusicFilePath.Size = new System.Drawing.Size(251, 22);
            this.uxMusicFilePath.TabIndex = 1;
            // 
            // openFileDialog
            // 
            this.openFileDialog.Filter = "Audio Files|*.mp3;*.wav;*.mp3;*.mp1;*.ogg|All files|*.*";
            // 
            // uxGenerate
            // 
            this.uxGenerate.Location = new System.Drawing.Point(186, 91);
            this.uxGenerate.Margin = new System.Windows.Forms.Padding(4);
            this.uxGenerate.Name = "uxGenerate";
            this.uxGenerate.Size = new System.Drawing.Size(100, 28);
            this.uxGenerate.TabIndex = 2;
            this.uxGenerate.Text = "Generate!";
            this.uxGenerate.UseVisualStyleBackColor = true;
            this.uxGenerate.Click += new System.EventHandler(this.uxGenerate_Click);
            // 
            // uxChooseAudioGroup
            // 
            this.uxChooseAudioGroup.Controls.Add(this.uxOutputLabel);
            this.uxChooseAudioGroup.Controls.Add(this.uxMusicLabel);
            this.uxChooseAudioGroup.Controls.Add(this.uxOutputFilePath);
            this.uxChooseAudioGroup.Controls.Add(this.uxBrowseOutput);
            this.uxChooseAudioGroup.Controls.Add(this.uxMsg);
            this.uxChooseAudioGroup.Controls.Add(this.uxMusicFilePath);
            this.uxChooseAudioGroup.Controls.Add(this.uxGenerate);
            this.uxChooseAudioGroup.Controls.Add(this.uxBrowseMusic);
            this.uxChooseAudioGroup.Location = new System.Drawing.Point(12, 12);
            this.uxChooseAudioGroup.Name = "uxChooseAudioGroup";
            this.uxChooseAudioGroup.Size = new System.Drawing.Size(476, 210);
            this.uxChooseAudioGroup.TabIndex = 3;
            this.uxChooseAudioGroup.TabStop = false;
            this.uxChooseAudioGroup.Text = "Choose Audio File";
            this.uxChooseAudioGroup.Enter += new System.EventHandler(this.uxChooseAudioGroup_Enter);
            // 
            // uxOutputLabel
            // 
            this.uxOutputLabel.Location = new System.Drawing.Point(6, 61);
            this.uxOutputLabel.Name = "uxOutputLabel";
            this.uxOutputLabel.Size = new System.Drawing.Size(87, 22);
            this.uxOutputLabel.TabIndex = 7;
            this.uxOutputLabel.Text = "Output:";
            // 
            // uxMusicLabel
            // 
            this.uxMusicLabel.Location = new System.Drawing.Point(6, 25);
            this.uxMusicLabel.Name = "uxMusicLabel";
            this.uxMusicLabel.Size = new System.Drawing.Size(87, 22);
            this.uxMusicLabel.TabIndex = 6;
            this.uxMusicLabel.Text = "Music File:";
            // 
            // uxOutputFilePath
            // 
            this.uxOutputFilePath.Location = new System.Drawing.Point(100, 61);
            this.uxOutputFilePath.Margin = new System.Windows.Forms.Padding(4);
            this.uxOutputFilePath.Name = "uxOutputFilePath";
            this.uxOutputFilePath.Size = new System.Drawing.Size(251, 22);
            this.uxOutputFilePath.TabIndex = 5;
            // 
            // uxBrowseOutput
            // 
            this.uxBrowseOutput.Location = new System.Drawing.Point(359, 58);
            this.uxBrowseOutput.Margin = new System.Windows.Forms.Padding(4);
            this.uxBrowseOutput.Name = "uxBrowseOutput";
            this.uxBrowseOutput.Size = new System.Drawing.Size(100, 28);
            this.uxBrowseOutput.TabIndex = 4;
            this.uxBrowseOutput.Text = "Browse ..";
            this.uxBrowseOutput.UseVisualStyleBackColor = true;
            this.uxBrowseOutput.Click += new System.EventHandler(this.uxBrowseOutput_Click);
            // 
            // uxMsg
            // 
            this.uxMsg.Location = new System.Drawing.Point(6, 123);
            this.uxMsg.Name = "uxMsg";
            this.uxMsg.Size = new System.Drawing.Size(464, 84);
            this.uxMsg.TabIndex = 3;
            this.uxMsg.Text = "Click browse to select MP3 file";
            this.uxMsg.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // statusStrip
            // 
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.uxProgress});
            this.statusStrip.Location = new System.Drawing.Point(0, 225);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(500, 26);
            this.statusStrip.TabIndex = 4;
            this.statusStrip.Text = "statusStrip1";
            // 
            // uxProgress
            // 
            this.uxProgress.Name = "uxProgress";
            this.uxProgress.Size = new System.Drawing.Size(400, 20);
            // 
            // saveFileDialog
            // 
            this.saveFileDialog.DefaultExt = "relips";
            this.saveFileDialog.Filter = "ReLips Stage File|*.relips";
            // 
            // errorProvider
            // 
            this.errorProvider.ContainerControl = this;
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(500, 251);
            this.Controls.Add(this.statusStrip);
            this.Controls.Add(this.uxChooseAudioGroup);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "FormMain";
            this.Text = "Automatic Karaoke-Game Stage Generation";
            this.Load += new System.EventHandler(this.FormMain_Load);
            this.uxChooseAudioGroup.ResumeLayout(false);
            this.uxChooseAudioGroup.PerformLayout();
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button uxBrowseMusic;
        private System.Windows.Forms.TextBox uxMusicFilePath;
        private System.Windows.Forms.OpenFileDialog openFileDialog;
        private System.Windows.Forms.Button uxGenerate;
        private System.Windows.Forms.GroupBox uxChooseAudioGroup;
        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.Label uxMsg;
        private System.Windows.Forms.ToolStripProgressBar uxProgress;
        private System.Windows.Forms.SaveFileDialog saveFileDialog;
        private System.Windows.Forms.Label uxMusicLabel;
        private System.Windows.Forms.TextBox uxOutputFilePath;
        private System.Windows.Forms.Button uxBrowseOutput;
        private System.Windows.Forms.Label uxOutputLabel;
        private System.Windows.Forms.ErrorProvider errorProvider;
    }
}

