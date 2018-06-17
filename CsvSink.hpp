//
// Created by unitelabs on 16/06/18.
//
// in file Customsink.hpp
#ifndef CSV_SINK_hpp
#define CSV_SINK_hpp

#include <string>
#include <iostream>
#include <g3log/logmessage.hpp>


class CsvSink {
public:
    CsvSink(const std::string csv_file):
    _full_file_path(csv_file) {
        _outptr = createLogFile(csv_file);
    }

    virtual ~CsvSink() {
        std::string exit_msg {"g3log CsvSink shutdown"};
        _outptr.get()->close();
        std::cerr << exit_msg << std::flush;
    };

    std::string fileName() {
        return _full_file_path;
    }
    template <typename T>
    void write(T t)
    {
        *_outptr.get() << t << std::endl ;
    }

    template<typename T, typename... Args>
    void write(T t, Args... args) // recursive variadic function
    {
        *_outptr.get() << t << ";";
        write(args...);
    }

    /**
     * Not really ever used, write is rather used
     * @param csvLine
     */
    void receiveCsvLine(std::string csvLine) {
        *_outptr.get() << csvLine;
    }

    std::ofstream& stream() {return *_outptr.get();}

private:
    std::unique_ptr<std::ofstream> _outptr;
    std::string _full_file_path;

    std::unique_ptr<std::ofstream> createLogFile(const std::string &file_with_full_path) {
        std::unique_ptr<std::ofstream> out(new std::ofstream);
        std::ofstream &stream(*(out.get()));
        stream.open(file_with_full_path, std::ios_base::out);
        if (!stream.is_open()) {
            std::cerr << "ERROR: Failed to open file: " << file_with_full_path;
            out.release();
        }
        return out;
    }
};
#endif /* CSV_SINK_hpp */
