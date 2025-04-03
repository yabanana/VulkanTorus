// Application.h
#ifndef APPLICATION_H
#define APPLICATION_H

class Application {
public:
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;
    virtual ~Application() {}
};

#endif // APPLICATION_H
