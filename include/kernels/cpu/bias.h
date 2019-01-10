#ifndef TS_KERNELS_BIAS_H
#define TS_KERNELS_BIAS_H

#include <global/operator_factory.h>
#include <core/tensor.h>
#include <runtime/stack.h>
//#include <cstring>
//#include <math.h>



namespace ts {


class Bias : public ts::Operator {
public:

    using supper = ts::Operator;
    Bias();

    virtual void init(); 

    virtual int run(ts::Stack &stack);
    virtual int infer(ts::Stack &stack, std::vector<ts::Tensor::Prototype> &output); 


private:
    template<typename T>
    void compute_bias(ts::Tensor *input_tensor, ts::Tensor *bias_tensor, ts::Tensor *output_tensor);
    void infer_private(ts::Stack &stack, ts::Tensor::Prototype &output);

private:
    int m_dim;

};


TS_REGISTER_OPERATOR(Bias, ts::CPU, "bias")


}

#endif
