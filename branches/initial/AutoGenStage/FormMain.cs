using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using ReLipsLib;
using System.Threading;
using System.Text.RegularExpressions;

namespace AutoGenStage
{
    public partial class FormMain : Form
    {
        StageGenerator stageGen;

        public FormMain()
        {
            InitializeComponent();

            stageGen = new StageGenerator();
            stageGen.OnProgress += new GenerationProgressHandler(sg_OnProgress);
            stageGen.OnCompleted += new GenerationCompletedHandler(sg_OnCompleted);
        }

        private void uxBrowseMusic_Click(object sender, EventArgs e)
        {
            if(openFileDialog.ShowDialog() == DialogResult.OK){
                uxMusicFilePath.Text = openFileDialog.FileName;
            }
        }

        private void uxBrowseOutput_Click(object sender, EventArgs e)
        {
           if(saveFileDialog.ShowDialog() == DialogResult.OK){
               uxOutputFilePath.Text = saveFileDialog.FileName;
           }
        }

        private void uxGenerate_Click(object sender, EventArgs e)
        {
            bool isValid = true;
            if (!IsValidPath(uxMusicFilePath.Text))
            {
                errorProvider.SetError(uxMusicFilePath, "Invalid path");
                isValid = false;
            }
            else errorProvider.SetError(uxMusicFilePath, "");

            if (!IsValidPath(uxOutputFilePath.Text))
            {
                errorProvider.SetError(uxOutputFilePath, "Invalid path");
                isValid = false;
            }
            else errorProvider.SetError(uxOutputFilePath, "");

            if (!isValid)
                return;

            uxGenerate.Enabled = false;
            stageGen.CreateStageFromFileAsync(uxMusicFilePath.Text, uxOutputFilePath.Text);
        }

        public void ShowStatusMessage(string text){
            if (this.InvokeRequired)
            {
                this.Invoke(new MethodInvoker(delegate()
                {
                    ShowStatusMessage(text);
                }));
            }
            else
            {
                uxMsg.Text = text;
            }
        }

        void ChangeUIProgess(int percent){
            if (this.InvokeRequired)
            {
                this.Invoke(new MethodInvoker(delegate()
                {
                    ChangeUIProgess(percent);
                }));
            }
            else
            {
                uxProgress.Value = percent;
            }
        }

        void ChangeUIBeginGenerate(){
            if (this.InvokeRequired)
            {
                this.Invoke(new MethodInvoker(delegate()
                {
                    ChangeUIBeginGenerate();
                }));
            }
            else
            {
                uxGenerate.Enabled = false;
            }
        }

        void ChangeUIFinishGenerate()
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new MethodInvoker(delegate()
                {
                    ChangeUIFinishGenerate();
                }));
            }
            else
            {
                uxGenerate.Enabled = true;
                uxProgress.Value = 0;
            }
        }

        void sg_OnProgress(object sender, GenerationProgressArgs args)
        {
            ShowStatusMessage("Generating.. Please wait " + Environment.NewLine +
                                args.PercentageCompleted() + "%" + Environment.NewLine + 
                                args.Message());
            int p = args.PercentageCompleted();
            p = (p > 100) ? 100 : (p < 0) ? 0 : p;
            ChangeUIProgess(p);
        }

        void sg_OnCompleted(object sender, GenerationCompletedArgs args)
        {
            if (args.HasException())
            {
                ShowStatusMessage("Failed" + Environment.NewLine +
                                   args.InnerException().Message);
            }
            else
            {
                ShowStatusMessage("Completed!");
            }

            ChangeUIFinishGenerate();
        }

        static public bool IsValidPath(string path)
        {
            Regex r = new Regex(@"^(([a-zA-Z]\:)|(\\))(\\{1}|((\\{1})[^\\]([^/:*?<>""|]*))+)$");
            return r.IsMatch(path);
        }

        private void uxChooseAudioGroup_Enter(object sender, EventArgs e)
        {

        }

        private void FormMain_Load(object sender, EventArgs e)
        {

        }


    }
}
