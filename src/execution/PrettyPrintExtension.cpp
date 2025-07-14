#include "lingodb/execution/PrettyPrintExtension.h"


arrow::Status lingodb::execution::ExtensionPrinter::PrettyPrint(const arrow::ChunkedArray& chunked_arr, arrow::PrettyPrintOptions options, std::string* target) {
    this->target = target;
    this->options = options;
    this->target->append("[");
    for (int i = 0; i < chunked_arr.num_chunks(); ++i) {
        this->target->append("\n[");
        auto chunk = chunked_arr.chunk(i);
        ARROW_RETURN_NOT_OK(arrow::VisitArrayInline(*chunk, this));
        if (i + 1 == chunked_arr.num_chunks()) {
            this->target->append("]");
        } else {
            this->target->append("],");
        }
        
    }
    this->target->append("\n]");
    return arrow::Status::OK();
}

arrow::Status lingodb::execution::ExtensionPrinter::Visit(const arrow::HalfFloatArray& array) {
    unsigned index = 0;
    union lingodb::execution::HalfFloatValue halfFloatValue;
    bool setDots = false;
    for (std::optional<typename arrow::HalfFloatType::c_type> value : array) {
        if (value.has_value() && (index < options.window || index >= (array.length() - options.window))) {
            halfFloatValue.asInt = static_cast<unsigned>(value.value()) << 16;
            this->target->append("\n");
            this->target->append(std::to_string(halfFloatValue.asFloat));
            if (index < array.length() - 1) {
               this->target->append(",");
            } else {
               this->target->append("\n");
            }
        } else if (value.has_value() && !setDots) {
            this->target->append("\n");
            this->target->append("...");
            setDots = true;
        }
        ++index;
    }
    return arrow::Status::OK();
}

arrow::Status lingodb::execution::ExtensionPrinter::Visit(const arrow::Array& array) {
    return arrow::Status::NotImplemented("Can not compute sum for array of type ", array.type()->ToString());
}