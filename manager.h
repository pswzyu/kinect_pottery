#ifndef MANAGER_H
#define MANAGER_H

class Manager
{
public:
	bool should_run;

    Manager();
    virtual ~Manager();
    virtual void updateData();

};

#endif // MANAGER_H
