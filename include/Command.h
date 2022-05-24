class Command
{
public:
    virtual void start() = 0;

    virtual void execute() = 0;

    virtual bool continues() = 0;
};