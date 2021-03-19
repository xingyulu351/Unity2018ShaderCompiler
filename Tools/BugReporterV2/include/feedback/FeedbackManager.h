namespace ureport
{
    class FeedbackManager
    {
    public:
        virtual void GiveFeedback(const Report& report, std::function<void(std::map<std::string, Feedback>)> callback) = 0;
        virtual bool ShouldSendBeBlocked() = 0;
        virtual bool ShouldBugReporterBeDisabled() = 0;
        virtual ~FeedbackManager() {}
    };
}
