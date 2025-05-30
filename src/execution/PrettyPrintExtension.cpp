#include "lingodb/execution/PrettyPrintExtension.h"


arrow::Status lingodb::execution::ExtensionPrinter::PrettyPrint(const arrow::ChunkedArray& chunked_arr, std::ostream* sink) {
    this->sink = sink;
    *(this->sink) << "[";
    for (int i = 0; i < chunked_arr.num_chunks(); ++i) {
        *(this->sink) << "\n[";
        auto chunk = chunked_arr.chunk(i);
        ARROW_RETURN_NOT_OK(arrow::VisitArrayInline(*chunk, this));
        if (i + 1 == chunked_arr.num_chunks()) {
            *(this->sink) << "]";
        } else {
            *(this->sink) << "],";
        }
        
    }
    *(this->sink) << "\n]";
    return arrow::Status::OK();
}

arrow::Status lingodb::execution::ExtensionPrinter::Visit(const arrow::HalfFloatArray& array) {
    unsigned index = 0;
    union lingodb::execution::HalfFloatValue halfFloatValue;
    for (std::optional<typename arrow::HalfFloatType::c_type> value : array) {
       if (value.has_value()) {
          halfFloatValue.asInt = static_cast<unsigned>(value.value()) << 16;
          *sink << "\n";
          *sink << halfFloatValue.asFloat;
          if (index < array.length() - 1) {
             *sink << ",";
          } else {
             *sink << "\n";
          }
          ++index;
       }
    }
    return arrow::Status::OK();
}

arrow::Status lingodb::execution::ExtensionPrinter::Visit(const arrow::Array& array) {
    return arrow::Status::NotImplemented("Can not compute sum for array of type ", array.type()->ToString());
}