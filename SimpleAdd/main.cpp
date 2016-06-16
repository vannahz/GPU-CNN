#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <CL/cl.h>//����CL��ͷ�ļ�
  
using namespace std;
  
//��ά����
#define elements 4
  
//���ⲿ�ļ���ȡcl�ں˴���
bool GetFileData(const char* fname,string& str)
{
    FILE* fp = fopen(fname,"r");
    if(fp==NULL)
    {
        printf("no found file\n");
        return false;
    }
  
    int n=0;
    while(feof(fp)==0)
    {
        str += fgetc(fp);
    }
  
    return true;
}
  
int main()
{
    //�ȶ��ⲿCL���Ĵ��룬���ʧ�����˳���
    //�����buf_code����
    string code_file;
  
    if(false == GetFileData("vecadd.cl",code_file))
        return 0;
  
    char* buf_code = new char[code_file.size()];
    strcpy(buf_code,code_file.c_str());
    buf_code[code_file.size()-1] = NULL;
  
    //����CL���������
    cl_device_id device;
    cl_platform_id platform_id = NULL;
    cl_context context;
    cl_command_queue cmdQueue;
    cl_mem bufferA,bufferB,bufferC;
    cl_program program;
    cl_kernel kernel = NULL;
  
    //����ʹ�õ���һά����
    //�趨������С��ά����
    size_t globalWorkSize[1];
    globalWorkSize[0] = elements;
  
    cl_int err;
  
    /*
        �������������������������趨��ֵ
    */
    int* buf_A = new int[elements];
    int* buf_B = new int[elements];
    int* buf_C = new int[elements];
  
    size_t datasize = sizeof(int) * elements;
  
    buf_A[0] = 1;
    buf_A[1] = 2;
    buf_A[2] = 3;
    buf_A[3] = 4;
  
    buf_B[0] = 5;
    buf_B[1] = 6;
    buf_B[2] = 7;
    buf_B[3] = 8;
  
    //step 1:��ʼ��OpenCL
    err = clGetPlatformIDs(1,&platform_id,NULL);
  
    if(err!=CL_SUCCESS)
    {
        cout<<"clGetPlatformIDs error"<<endl;
        return 0;
    }
  
    //�������ֻ��CPU�����в������㣬��Ȼ��Ҳ���Ըó�GPU
    clGetDeviceIDs(platform_id,CL_DEVICE_TYPE_GPU,1,&device,NULL);
  
    //step 2:����������
    context = clCreateContext(NULL,1,&device,NULL,NULL,NULL);
  
    //step 3:�����������
    cmdQueue = clCreateCommandQueue(context,device,0,NULL);
  
    //step 4:�������ݻ�����
    bufferA = clCreateBuffer(context,
                             CL_MEM_READ_ONLY,
                             datasize,NULL,NULL);
  
    bufferB = clCreateBuffer(context,
                             CL_MEM_READ_ONLY,
                             datasize,NULL,NULL);
  
    bufferC = clCreateBuffer(context,
                             CL_MEM_WRITE_ONLY,
                             datasize,NULL,NULL);
  
    //step 5:�������ϴ���������
    clEnqueueWriteBuffer(cmdQueue,
                         bufferA,CL_FALSE,
                         0,datasize,
                         buf_A,0,
                         NULL,NULL);
  
    clEnqueueWriteBuffer(cmdQueue,
                         bufferB,CL_FALSE,
                         0,datasize,
                         buf_B,0,
                         NULL,NULL);
  
    //step 6:���ر������,�����ں˵��ú���
  
    program = clCreateProgramWithSource(context,1,
                                        (const char**)&buf_code,
                                        NULL,NULL);
  
    clBuildProgram(program,1,&device,NULL,NULL,NULL);
  
    kernel = clCreateKernel(program,"vecAdd",NULL);
  
    //step 7:���ò�����ִ���ں�
    clSetKernelArg(kernel,0,sizeof(cl_mem),&bufferA);
    clSetKernelArg(kernel,1,sizeof(cl_mem),&bufferB);
    clSetKernelArg(kernel,2,sizeof(cl_mem),&bufferC);
  
    clEnqueueNDRangeKernel(cmdQueue,kernel,
                           1,NULL,
                           globalWorkSize,
                           NULL,0,NULL,NULL);
  
    //step 8:ȡ�ؼ�����
    clEnqueueReadBuffer(cmdQueue,bufferC,CL_TRUE,0,
                        datasize,buf_C,0,NULL,NULL);
  
    //���������
    cout<<"["<<buf_A[0]<<","<<buf_A[1]<<","<<buf_A[2]<<","<<buf_A[3]<<"]+["
    <<buf_B[0]<<","<<buf_B[1]<<","<<buf_B[2]<<","<<buf_B[3]<<"]=["
    <<buf_C[0]<<","<<buf_C[1]<<","<<buf_C[2]<<","<<buf_C[3]<<"]"<<endl;
  
    //�ͷ����е��ú��ڴ�
	
  
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseContext(context);
  // printf("here!");
    delete buf_A;
    delete buf_B;
    delete buf_C;
    delete buf_code;
  
    return 0;
}