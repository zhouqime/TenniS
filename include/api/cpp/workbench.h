//
// Created by kier on 2019/3/16.
//

#ifndef TENSORSTACK_API_CPP_WORKBENCH_H
#define TENSORSTACK_API_CPP_WORKBENCH_H

#include "../workbench.h"

#include "except.h"
#include "device.h"
#include "module.h"
#include "tensor.h"
#include "image_filter.h"

#include <string>

namespace ts {
    namespace api {
        class Workbench {
        public:
            using self = Workbench;
            using raw = ts_Workbench;

            using shared = std::shared_ptr<self>;
            using shared_raw = std::shared_ptr<raw>;

            Workbench(const self &) = default;

            Workbench &operator=(const self &) = default;

            raw *get_raw() const { return m_impl.get(); }

            Workbench() = delete;

            static Workbench Load(const Module &module, const Device &device) {
                return Load(module.get_raw(), device.get_raw());
            }

            static Workbench Load(const ts_Module *module, const Device &device) {
                return Load(module, device.get_raw());
            }

            static Workbench Load(const Module &module, const ts_Device *device) {
                return Load(module.get_raw(), device);
            }

            static Workbench Load(const ts_Module *module, const ts_Device *device) {
                Workbench loaded(ts_Workbench_Load(module, device));
                TS_API_AUTO_CHECK(loaded.m_impl != nullptr);
                return std::move(loaded);
            }

            Workbench clone() const {
                Workbench dolly(ts_Workbench_clone(m_impl.get()));
                TS_API_AUTO_CHECK(dolly.m_impl != nullptr);
                return std::move(dolly);
            }

            void input(int slot, const ts_Tensor *tensor) {
                TS_API_AUTO_CHECK(ts_Workbench_input(m_impl.get(), slot, tensor));
            }

            void input(int slot, const Tensor &tensor) {
                input(slot, tensor.get_raw());
            }

            void input(const std::string &name, const ts_Tensor *tensor) {
                TS_API_AUTO_CHECK(ts_Workbench_input_by_name(m_impl.get(), name.c_str(), tensor));
            }

            void input(const std::string &name, const Tensor &tensor) {
                input(name, tensor.get_raw());
            }

            void run() {
                TS_API_AUTO_CHECK(ts_Workbench_run(m_impl.get()));
            }

            void output(int slot, ts_Tensor *tensor) {
                TS_API_AUTO_CHECK(ts_Workbench_output(m_impl.get(), slot, tensor));
            }

            void output(int slot, const Tensor &tensor) {
                output(slot, tensor.get_raw());
            }

            void output(const std::string &name, ts_Tensor *tensor) {
                TS_API_AUTO_CHECK(ts_Workbench_output_by_name(m_impl.get(), name.c_str(), tensor));
            }

            void output(const std::string &name, Tensor &tensor) {
                output(name, tensor.get_raw());
            }

            Tensor output(int slot) {
                Tensor tensor;
                output(slot, tensor);
                return std::move(tensor);
            }

            Tensor output(const std::string &name) {
                Tensor tensor;
                output(name, tensor);
                return std::move(tensor);
            }

            void set_computing_thread_number(int number) {
                TS_API_AUTO_CHECK(ts_Workbench_set_computing_thread_number(m_impl.get(), number));
            }

            void bind_filter(int slot, const ts_ImageFilter *filter) {
                TS_API_AUTO_CHECK(ts_Workbench_bind_filter(m_impl.get(), slot, filter));
            }

            void bind_filter(int slot, const ImageFilter &tensor) {
                bind_filter(slot, tensor.get_raw());
            }

            void bind_filter(const std::string &name, const ts_ImageFilter *filter) {
                TS_API_AUTO_CHECK(ts_Workbench_bind_filter_by_name(m_impl.get(), name.c_str(), filter));
            }

            void bind_filter(const std::string &name, const ImageFilter &tensor) {
                bind_filter(name, tensor.get_raw());
            }

        private:
            Workbench(raw *ptr) : m_impl(pack(ptr)) {}

            static shared_raw pack(raw *ptr) { return shared_raw(ptr, ts_free_Workbench); }

            shared_raw m_impl;
        };
    }
}

#endif //TENSORSTACK_API_CPP_WORKBENCH_H