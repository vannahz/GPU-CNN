__kernel void vecAdd(__global int* A,
        __global int* B,
        __global int* C)
{
    //��ȡ��ǰ����������λ�ã��߳������ţ�
    int idx = get_global_id(0);
    C[idx] = A[idx] + B[idx];
}