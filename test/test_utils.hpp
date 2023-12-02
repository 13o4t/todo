#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>

class CapturingStdout {
public:
    CapturingStdout(CapturingStdout const&) = delete;
    CapturingStdout(CapturingStdout&&) = delete;
    CapturingStdout& operator=(CapturingStdout const&) = delete;
    CapturingStdout& operator=(CapturingStdout&&) = delete;

    CapturingStdout() {
        prevcoutbuf_ = std::cout.rdbuf(buffer_.rdbuf());
    }
    ~CapturingStdout() {
        std::cout.rdbuf(prevcoutbuf_);
    }

    std::string contents() {
        return buffer_.str();
    }

private:
    std::stringstream buffer_;
    std::streambuf* prevcoutbuf_;
};

class TempFile {
public:
    TempFile(TempFile const&) = delete;
    TempFile(TempFile&&) = delete;
    TempFile& operator=(TempFile const&) = delete;
    TempFile& operator=(TempFile&&) = delete;

    TempFile(const std::string& suffix, const std::string& name = "") {
        std::string unique = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());

        filename_ = std::filesystem::temp_directory_path() / "todo_test";
        if (!name.empty()) filename_ /= name + suffix;
        else filename_ /= unique + suffix;
        std::filesystem::create_directories(filename_.parent_path());

        file_.open(filename_, std::ios::in | std::ios::out | std::ios::trunc);
        if (!file_.is_open()) throw std::runtime_error("cannot create temp file " + filename_.string());
        file_.close();
    }

    ~TempFile() {
        file_.close();
        std::remove(filename_.string().c_str());
        std::remove(filename_.parent_path().string().c_str());
    }

    std::filesystem::path filename() { return filename_; }
    std::fstream& file() { return file_; }

    void open() { if (!file_.is_open()) file_.open(filename_, std::ios::in | std::ios::out); }
    void close() { file_.close(); }

    std::string contents() {
        bool is_open = file_.is_open();
        open();
        std::string contents = std::string((std::istreambuf_iterator<char>(file_)), std::istreambuf_iterator<char>());
        if (!is_open) close();
        return contents;
    }

private:
    std::filesystem::path filename_;
    std::fstream file_;
};

class TestFile {
public:
    TestFile(TestFile const&) = delete;
    TestFile(TestFile&&) = delete;
    TestFile& operator=(TestFile const&) = delete;
    TestFile& operator=(TestFile&&) = delete;

    TestFile() : main_(TempFile(".txt", "todo")), bak_(TempFile(".txt.bak", "todo")), tmp_(TempFile(".txt.tmp", "todo")) {
    }

    TempFile& main() { return main_; }
    TempFile& bak() { return bak_; }
    TempFile& tmp() { return tmp_; }

private:
    TempFile main_;
    TempFile bak_;
    TempFile tmp_;
};