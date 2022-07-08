class Command
{
public:
    virtual void start();

    virtual void execute();

    virtual bool continues();
};