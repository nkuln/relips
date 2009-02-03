using AutoGenStage;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

namespace AutoGenStageTest
{
    
    
    /// <summary>
    ///This is a test class for FormTestTest and is intended
    ///to contain all FormTestTest Unit Tests
    ///</summary>
    [TestClass()]
    public class FormTestTest
    {


        private TestContext testContextInstance;

        /// <summary>
        ///Gets or sets the test context which provides
        ///information about and functionality for the current test run.
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        #region Additional test attributes
        // 
        //You can use the following additional attributes as you write your tests:
        //
        //Use ClassInitialize to run code before running the first test in the class
        //[ClassInitialize()]
        //public static void MyClassInitialize(TestContext testContext)
        //{
        //}
        //
        //Use ClassCleanup to run code after all tests in a class have run
        //[ClassCleanup()]
        //public static void MyClassCleanup()
        //{
        //}
        //
        //Use TestInitialize to run code before running each test
        //[TestInitialize()]
        //public void MyTestInitialize()
        //{
        //}
        //
        //Use TestCleanup to run code after each test has run
        //[TestCleanup()]
        //public void MyTestCleanup()
        //{
        //}
        //
        #endregion


        [TestMethod()]
        public void GetNoteTest()
        {
            FormTest target = new FormTest();
            List<FormTest.Fragment> list = new List<FormTest.Fragment>();
            list.Add(new FormTest.Fragment() { Start = 100, End = 200, Note = 1 });
            list.Add(new FormTest.Fragment() { Start = 200, End = 350, Note = 3 });
            list.Add(new FormTest.Fragment() { Start = 400, End = 500, Note = 5 });


            Assert.AreEqual(1,target.GetNote(list, 100, 200));
            Assert.AreEqual(3,target.GetNote(list, 200, 300));
            Assert.AreEqual(-1,target.GetNote(list, 10, 100));
            Assert.AreEqual(-1,target.GetNote(list, 350, 351));
            Assert.AreEqual(5,target.GetNote(list, 499, 500));
        }

        [TestMethod()]
        public void CompareTest()
        {
            FormTest target = new FormTest();
            string s = @"D:\RelipsProject\AutoGenStageTest\";
            double res = target.CompareExact(s+"test3.relips", s+"test1.relips");
            Assert.AreEqual(1.0,res);
        }

    }
}
