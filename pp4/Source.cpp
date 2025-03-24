#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <CL/cl.hpp>

#include "gpu_isomorphic.h"
#include "cpu_isomorphic.h"
#include "matrix.h"

void checkError(cl_int err, const char* operation) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error during operation '%s': %d\n", operation, err);
        exit(1);
    }
}

int main() {
    // ������ ������� ��� ����������
    int arr[] = { 12, 11, 13, 5, 6, 7 };
    int n = sizeof(arr) / sizeof(arr[0]);

    // ������������� OpenCL
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

    cl_mem arr_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        sizeof(arr), arr, NULL);

    // �������� � ���������� ����
    const char* kernelSource = /* �������� ����� ��� ���� */;
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);
    cl_int err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    checkError(err, "Building program");

    cl_kernel kernel = clCreateKernel(program, "heap_sort", &err);
    checkError(err, "Creating kernel");

    // ��������� ���������� ����
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &arr_buffer);
    err |= clSetKernelArg(kernel, 1, sizeof(int), &n);

    checkError(err, "Setting kernel arguments");

    // ������ ����
    size_t globalSize = 1; // �� ��������� ���� ���� ��� ����������
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);

    checkError(err, "Enqueuing kernel");

    // ���������� ���������� ������� � ����
    err = clEnqueueReadBuffer(queue, arr_buffer, CL_TRUE, 0, sizeof(arr), arr, 0, NULL, NULL);

    checkError(err, "Reading buffer");

    // ����� ���������������� �������
    printf("Sorted array: \n");
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);

    // ������������ ��������
    clReleaseMemObject(arr_buffer);
    clReleaseProgram(program);
