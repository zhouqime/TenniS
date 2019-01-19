#ifndef TS_KERNELS_ADD_H
#define TS_KERNELS_ADD_H

#include <core/tensor.h>
#include <runtime/stack.h>
#include <runtime/operator.h>


namespace ts {


class Add : public ts::Operator {
public:

    using supper = ts::Operator;
    Add();

    virtual void init(); 

    virtual int run(ts::Stack &stack);
    virtual int infer(ts::Stack &stack, std::vector<ts::Tensor::Prototype> &output); 


private:

    int to_index(const HypeShape &hype, const Shape & shape, const Shape &curshape);
    template<typename T>
    void compute_run(Tensor *input_tensor, Tensor *right_tensor,Tensor *left_tensor);
    void infer_private(ts::Stack &stack, ts::Tensor::Prototype &output);


};




}

#endif