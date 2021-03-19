using System;
using System.Collections.Generic;

namespace Unity.CommonTools
{
    /// <summary>
    /// Represents result of testing.
    /// </summary>
    public class TestResult
    {
        private State m_State = State.Inconclusive;
        private readonly string m_Name;
        private readonly List<TestResult> m_Results = new List<TestResult>();

        /// <summary>
        /// Represents state of the result.
        /// </summary>
        public enum State
        {
            /// <summary>
            /// Represents inconclusive result.
            /// </summary>
            Inconclusive,
            /// <summary>
            /// Represents successful result.
            /// </summary>
            Success,
            /// <summary>
            /// Represents failure result.
            /// </summary>
            Failure,
            /// <summary>
            /// Represents result failed with an error.
            /// </summary>
            Error,
            /// <summary>
            /// Represents an ignored test
            /// </summary>
            Ignored
        }

        /// <summary>
        /// Creates empty test result.
        /// </summary>
        public TestResult()
        {
        }

        /// <summary>
        /// Creates result for specified runtime test
        /// </summary>
        /// <param name="testName">Test name of the test</param>
        public TestResult(string testName)
        {
            m_Name = testName;
        }

        /// <summary>
        /// Gets state of the result.
        /// </summary>
        public State state
        {
            get { return m_State; }
        }

        /// <summary>
        /// Gets name assiciated with the test result.
        /// Can be a name of the test itself.
        /// </summary>
        public string name
        {
            get { return m_Name; }
        }

        /// <summary>
        /// Gets message assiciated with the result.
        /// </summary>
        public string message { get; private set; }

        /// <summary>
        /// Gets a value indicating whether the result contains one or more child results.
        /// </summary>
        public bool hasResults
        {
            get { return m_Results.Count > 0; }
        }

        /// <summary>
        /// Gets all results have been added as a child result.
        /// </summary>
        public IEnumerable<TestResult> results
        {
            get { return m_Results; }
        }

        /// <summary>
        /// Gets or sets test start time
        /// </summary>
        public DateTime started { get; set; }

        /// <summary>
        /// Gets or sets the time value associated with the test result. Milliseconds.
        /// </summary>
        public long time { get; set; }

        /// <summary>
        /// Sets result.
        /// </summary>
        /// <param name="newState">The state of result.</param>
        public void SetResult(State newState)
        {
            m_State = newState;
        }

        /// <summary>
        /// Sets result.
        /// </summary>
        /// <param name="newState">The state of result.</param>
        /// <param name="newMessage">The message related to the state of the result.</param>
        public void SetResult(State newState, string newMessage)
        {
            SetResult(newState);
            message = newMessage;
        }

        /// <summary>
        /// Sets the result to success state.
        /// </summary>
        public void Success()
        {
            SetResult(State.Success);
        }

        /// <summary>
        /// Sets the result to success state.
        /// </summary>
        /// <param name="newMessage">The message related to the result.</param>
        public void Success(string newMessage)
        {
            Success();
            message = newMessage;
        }

        /// <summary>
        /// Sets the result to fail and provide a message.
        /// </summary>
        /// <param name="newMessage">The message related to the result.</param>
        public void Fail(string newMessage)
        {
            SetResult(State.Failure);
            message = newMessage;
        }

        /// <summary>
        /// Adds specified result as a child.
        /// </summary>
        /// <param name="testResult">The result to be added. Can be null.</param>
        public void AddResult(TestResult testResult)
        {
            m_Results.Add(testResult);
        }
    }
}
