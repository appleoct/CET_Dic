//
// Created by kang on 17-10-2.
//

#ifndef KEDIS_JOBTEST_H
#define KEDIS_JOBTEST_H


#include "BaseJob.h"
#include "Factory.h"
class JobTest : public BaseJob{
public:
    JobTest() { std::cout << "init Test_Job" << std::endl;}
    int doJob(char *buffer) override ;
};

REGISTER_ACTION(JobTest, "TEST");


#endif //KEDIS_JOBTEST_H
