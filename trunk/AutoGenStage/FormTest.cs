using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace AutoGenStage
{
    public partial class FormTest : Form
    {
        public FormTest()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog dgTest = new OpenFileDialog();
            dgTest.Title = "Select Test File";
            if (dgTest.ShowDialog() == DialogResult.OK)
            {
                OpenFileDialog dgCorrect = new OpenFileDialog();
                dgCorrect.Title = "Select Correct File";
                if (dgCorrect.ShowDialog() == DialogResult.OK)
                {
                    try
                    {
                        double exact = CompareExact(dgTest.FileName, dgCorrect.FileName);
                        label1.Text = "Exact Accuracy = " + exact * 100.0 + "%" + Environment.NewLine;

                        double cover = CompareCover(dgTest.FileName, dgCorrect.FileName);
                        label1.Text += "Cover Accuracy = " + cover * 100.0 + "%" + Environment.NewLine; ;

                        double coverLoose = CompareCoverLoose(dgTest.FileName, dgCorrect.FileName);
                        label1.Text += "Cover (Loose) Accuracy = " + coverLoose * 100.0 + "%";
                    }
                    catch (Exception ex)
                    {
                        label1.Text = ex.Message;
                    }
                }
            }
        }

        public double CompareExact(string fileTest, string fileCorrect)
        {
            List<Fragment> test = FragmentListFromFile(fileTest);
            List<Fragment> correct = FragmentListFromFile(fileCorrect);

            // assume no overlapping between fragment within test and correct
            List<int> keyframes = new List<int>();
            int maxTest = int.MinValue, maxCorrect = int.MinValue;
            foreach (Fragment a in test)
            {
                if (!keyframes.Contains(a.Start))
                    keyframes.Add(a.Start);
                if (!keyframes.Contains(a.End))
                    keyframes.Add(a.End);
                if (a.End > maxTest) maxTest = a.End;
            }
            foreach (Fragment a in correct)
            {
                if (!keyframes.Contains(a.Start))
                    keyframes.Add(a.Start);
                if (!keyframes.Contains(a.End))
                    keyframes.Add(a.End);
                if (a.End > maxCorrect) maxCorrect = a.End;
            }

            int cutPoint = Math.Min(maxCorrect, maxTest);

            keyframes.Sort();

            keyframes.RemoveAll(delegate(int a) { return a > cutPoint; });

            if (keyframes.Count < 2)
                return 0.0;

            // compare

            int sum = 0;

            for (int i = 0; i < keyframes.Count - 1; i++)
            {
                int m = GetNote(test, keyframes[i], keyframes[i + 1]);
                int n = GetNote(correct, keyframes[i], keyframes[i + 1]);
                if (m == n)
                    sum += keyframes[i + 1] - keyframes[i];
            }

            int len = (keyframes[keyframes.Count - 1] - keyframes[0]);
            double acc = sum / (1.0 * len);
            return acc;
        }

        public double CompareCover(string fileTest, string fileCorrect)
        {
            List<Fragment> test = FragmentListFromFile(fileTest);
            List<Fragment> correct = FragmentListFromFile(fileCorrect);

            // assume no overlapping between fragment within test and correct
            List<int> keyframes = new List<int>();
            int maxTest = int.MinValue, maxCorrect = int.MinValue;
            foreach (Fragment a in test)
            {
                if (!keyframes.Contains(a.Start))
                    keyframes.Add(a.Start);
                if (!keyframes.Contains(a.End))
                    keyframes.Add(a.End);
                if (a.End > maxTest) maxTest = a.End;
            }
            foreach (Fragment a in correct)
            {
                if (!keyframes.Contains(a.Start))
                    keyframes.Add(a.Start);
                if (!keyframes.Contains(a.End))
                    keyframes.Add(a.End);
                if (a.End > maxCorrect) maxCorrect = a.End;
            }

            int cutPoint = Math.Min(maxCorrect, maxTest);

            keyframes.Sort();

            keyframes.RemoveAll(delegate(int a) { return a > cutPoint; });

            if (keyframes.Count < 2)
                return 0.0;

            // compare

            int sum = 0;
            int divider = 0;

            for (int i = 0; i < keyframes.Count - 1; i++)
            {
                int m = GetNote(test, keyframes[i], keyframes[i + 1]);
                int n = GetNote(correct, keyframes[i], keyframes[i + 1]);

                if (n != -1)
                { 
                    // singing voice
                    divider += keyframes[i + 1] - keyframes[i];

                    if (m == n)
                        sum += keyframes[i + 1] - keyframes[i];
                }
            }

            double acc = sum / (1.0 * divider);
            return acc;
        }

        public double CompareCoverLoose(string fileTest, string fileCorrect)
        {
            List<Fragment> test = FragmentListFromFile(fileTest);
            List<Fragment> correct = FragmentListFromFile(fileCorrect);

            // assume no overlapping between fragment within test and correct
            List<int> keyframes = new List<int>();
            int maxTest = int.MinValue, maxCorrect = int.MinValue;
            foreach (Fragment a in test)
            {
                if (!keyframes.Contains(a.Start))
                    keyframes.Add(a.Start);
                if (!keyframes.Contains(a.End))
                    keyframes.Add(a.End);
                if (a.End > maxTest) maxTest = a.End;
            }
            foreach (Fragment a in correct)
            {
                if (!keyframes.Contains(a.Start))
                    keyframes.Add(a.Start);
                if (!keyframes.Contains(a.End))
                    keyframes.Add(a.End);
                if (a.End > maxCorrect) maxCorrect = a.End;
            }

            int cutPoint = Math.Min(maxCorrect, maxTest);

            keyframes.Sort();

            keyframes.RemoveAll(delegate(int a) { return a > cutPoint; });

            if (keyframes.Count < 2)
                return 0.0;

            // compare

            int sum = 0;
            int divider = 0;

            for (int i = 0; i < keyframes.Count - 1; i++)
            {
                int m = GetNote(test, keyframes[i], keyframes[i + 1]);
                int n = GetNote(correct, keyframes[i], keyframes[i + 1]);

                if (n != -1)
                {
                    // singing voice
                    divider += keyframes[i + 1] - keyframes[i];

                    int p = m, q = m;

                    if (p == 5) p = 7;
                    else if (p == 13) p = 1;
                    else p++;

                    if (q == 7) q = 5;
                    else if (q == 1) q = 13;
                    else q--;
                    
                    if (m == n || q == n || p == n)
                        sum += keyframes[i + 1] - keyframes[i];
                }
            }

            double acc = sum / (1.0 * divider);
            return acc;
        }

        public int GetNote(List<Fragment> test, int start, int end)
        {
            foreach (Fragment m in test)
            {
                if (start >= m.Start && end <= m.End)
                    return m.Note;
                if (start <= m.Start)
                    return -1;
            }
            return -1;
        }

        public List<Fragment> FragmentListFromFile(string fileTest)
        {
            TextReader tr = new StreamReader(fileTest);
            List<Fragment> ret = new List<Fragment>();
            string s;
            while ((s = tr.ReadLine()) != null)
            {
                s = s.Trim();
                if (s.Length != 0)
                {
                    string[] rec = s.Split(' ', '\t', '\r', '\n');
                    if (rec.Length != 3)
                        throw new ApplicationException("Invalid File Format");

                    int start = Convert.ToInt32(rec[0]);
                    int end = Convert.ToInt32(rec[1]);
                    int note = Convert.ToInt32(rec[2]);

                    Fragment m = new Fragment() { Start = start, End = end, Note = note };
                    ret.Add(m);
                }
            }

            ret.Sort(delegate(Fragment a, Fragment b)
            {
                return a.Start.CompareTo(b.Start);
            });

            return ret;
        }

        public class Fragment
        {
            public int Start { get; set; }
            public int End { get; set; }
            public int Note { get; set; }
        }
    }


}
