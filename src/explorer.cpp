#include "explorer.hpp"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>

namespace fs = std::filesystem;

Explorer::Explorer() {
    cwd = fs::current_path().string();
}

std::vector<std::string> Explorer::split(const std::string& s) {
    std::istringstream iss(s);
    std::vector<std::string> tokens;
    std::string t;
    while (iss >> std::quoted(t)) { // allows quoted args
        tokens.push_back(t);
    }
    if (tokens.empty()) {
        // fallback: split by spaces if quoted extraction gave nothing
        std::istringstream iss2(s);
        while (iss2 >> t) tokens.push_back(t);
    }
    return tokens;
}

void Explorer::run() {
    std::string line;
    while (true) {
        std::cout << "[fe] " << cwd << " $ ";
        if (!std::getline(std::cin, line)) break;
        auto parts = split(line);
        if (parts.empty()) continue;
        std::string cmd = parts[0];
        parts.erase(parts.begin());

        if (cmd == "ls") cmd_ls(parts);
        else if (cmd == "cd") cmd_cd(parts);
        else if (cmd == "pwd") cmd_pwd(parts);
        else if (cmd == "cp") cmd_cp(parts);
        else if (cmd == "mv") cmd_mv(parts);
        else if (cmd == "rm") cmd_rm(parts);
        else if (cmd == "mkdir") cmd_mkdir(parts);
        else if (cmd == "touch") cmd_touch(parts);
        else if (cmd == "chmod") cmd_chmod(parts);
        else if (cmd == "search") cmd_search(parts);
        else if (cmd == "info") cmd_info(parts);
        else if (cmd == "help") cmd_help(parts);
        else if (cmd == "clear") cmd_clear(parts);
        else if (cmd == "exit" || cmd == "quit") break;
        else std::cout << "Unknown command: " << cmd << " (type 'help')\n";
    }
}

// ---------- command implementations ----------

void Explorer::ls_path(const std::string& path, bool show_hidden) {
    try {
        for (auto &entry : fs::directory_iterator(path)) {
            std::string name = entry.path().filename().string();
            if (!show_hidden && !name.empty() && name[0]=='.') continue;
            auto st = entry.status();
            if (fs::is_directory(st)) std::cout << "d ";
            else if (fs::is_symlink(st)) std::cout << "l ";
            else std::cout << "- ";
            auto ftime = fs::last_write_time(entry.path());
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - fs::file_time_type::clock::now()
                + std::chrono::system_clock::now()
            );
            std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
            std::cout << std::put_time(std::localtime(&cftime), "%F %T") << " ";
            print_permissions(st.permissions());
            std::cout << " " << name << "\n";
        }
    } catch (std::exception &e) {
        std::cout << "ls error: " << e.what() << "\n";
    }
}

void Explorer::cmd_ls(const std::vector<std::string>& args) {
    bool show_hidden = false;
    std::string target = cwd;
    for (auto &a : args) {
        if (a == "-a") show_hidden = true;
        else target = a;
    }
    if (!fs::exists(target)) {
        std::cout << "ls: path does not exist: " << target << "\n";
        return;
    }
    ls_path(target, show_hidden);
}

void Explorer::cmd_cd(const std::vector<std::string>& args) {
    if (args.empty()) {
        cwd = fs::path(getenv("HOME") ? getenv("HOME") : "/").string();
        return;
    }
    fs::path p = args[0];
    if (!p.is_absolute()) p = fs::path(cwd) / p;
    try {
        if (!fs::exists(p) || !fs::is_directory(p)) {
            std::cout << "cd: not a directory: " << p << "\n";
            return;
        }
        cwd = fs::canonical(p).string();
    } catch (std::exception &e) {
        std::cout << "cd error: " << e.what() << "\n";
    }
}

void Explorer::cmd_pwd(const std::vector<std::string>&) {
    std::cout << cwd << "\n";
}

bool Explorer::copy_recursive(const std::string& src, const std::string& dst) {
    try {
        fs::path s(src), d(dst);
        if (!s.is_absolute()) s = fs::path(cwd) / s;
        if (!d.is_absolute()) d = fs::path(cwd) / d;

        if (!fs::exists(s)) {
            std::cout << "cp: source does not exist: " << s << "\n";
            return false;
        }
        if (fs::is_directory(s)) {
            fs::create_directories(d);
            for (auto &p : fs::recursive_directory_iterator(s)) {
                auto rel = fs::relative(p.path(), s);
                auto target = d / rel;
                if (fs::is_directory(p.path())) fs::create_directories(target);
                else fs::copy_file(p.path(), target, fs::copy_options::overwrite_existing);
            }
        } else {
            fs::create_directories(d.parent_path());
            fs::copy_file(s, d, fs::copy_options::overwrite_existing);
        }
        return true;
    } catch (std::exception &e) {
        std::cout << "cp error: " << e.what() << "\n";
        return false;
    }
}

void Explorer::cmd_cp(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: cp <source> <destination>\n";
        return;
    }
    std::string src = args[0], dst = args[1];
    if (copy_recursive(src, dst)) std::cout << "Copied.\n";
}

void Explorer::cmd_mv(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: mv <source> <destination>\n";
        return;
    }
    fs::path s = args[0], d = args[1];
    if (!s.is_absolute()) s = fs::path(cwd) / s;
    if (!d.is_absolute()) d = fs::path(cwd) / d;
    try {
        fs::create_directories(d.parent_path());
        fs::rename(s, d);
        std::cout << "Moved.\n";
    } catch (std::exception &e) {
        std::cout << "mv error: " << e.what() << "\n";
    }
}

void Explorer::cmd_rm(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: rm <path> [-r]\n";
        return;
    }
    bool recursive = false;
    std::string target;
    for (auto &a : args) {
        if (a == "-r" || a == "-R") recursive = true;
        else target = a;
    }
    if (target.empty()) {
        std::cout << "rm: missing operand\n";
        return;
    }
    fs::path p = target;
    if (!p.is_absolute()) p = fs::path(cwd) / p;
    try {
        if (fs::is_directory(p) && !recursive) {
            std::cout << "rm: " << p << " is a directory (use -r)\n";
            return;
        }
        if (recursive) {
            fs::remove_all(p);
            std::cout << "Removed recursively.\n";
        } else {
            fs::remove(p);
            std::cout << "Removed.\n";
        }
    } catch (std::exception &e) {
        std::cout << "rm error: " << e.what() << "\n";
    }
}

void Explorer::cmd_mkdir(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: mkdir <dirname>\n";
        return;
    }
    fs::path p = args[0];
    if (!p.is_absolute()) p = fs::path(cwd) / p;
    try {
        fs::create_directories(p);
        std::cout << "Directory created: " << p << "\n";
    } catch (std::exception &e) {
        std::cout << "mkdir error: " << e.what() << "\n";
    }
}

void Explorer::cmd_touch(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: touch <filename>\n";
        return;
    }
    fs::path p = args[0];
    if (!p.is_absolute()) p = fs::path(cwd) / p;
    try {
        std::ofstream ofs(p.string(), std::ios::app);
        ofs.close();
        auto now = std::filesystem::file_time_type::clock::now();
        fs::last_write_time(p, now);
        std::cout << "Touched: " << p << "\n";
    } catch (std::exception &e) {
        std::cout << "touch error: " << e.what() << "\n";
    }
}

void Explorer::cmd_chmod(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: chmod <octal> <file>\n";
        return;
    }
    try {
        int mode = std::stoi(args[0], nullptr, 8);
        fs::path p = args[1];
        if (!p.is_absolute()) p = fs::path(cwd) / p;
        fs::permissions(p, static_cast<fs::perms>(mode), fs::perm_options::replace);
        std::cout << "Permissions changed.\n";
    } catch (std::exception &e) {
        std::cout << "chmod error: " << e.what() << "\n";
    }
}

void Explorer::cmd_search(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: search <pattern> [path]\n";
        return;
    }
    std::string pattern = args[0];
    fs::path start = cwd;
    if (args.size() > 1) start = args[1];
    if (!start.is_absolute()) start = fs::path(cwd) / start;
    try {
        for (auto &p : fs::recursive_directory_iterator(start)) {
            if (p.path().filename().string().find(pattern) != std::string::npos) {
                std::cout << p.path().string() << "\n";
            }
        }
    } catch (std::exception &e) {
        std::cout << "search error: " << e.what() << "\n";
    }
}

void Explorer::cmd_info(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: info <file>\n";
        return;
    }
    fs::path p = args[0];
    if (!p.is_absolute()) p = fs::path(cwd) / p;
    try {
        if (!fs::exists(p)) { std::cout << "info: does not exist\n"; return; }
        auto st = fs::status(p);
        std::cout << "Path: " << fs::absolute(p) << "\n";
        std::cout << "Size: " << (fs::is_regular_file(p) ? fs::file_size(p) : 0) << "\n";
        std::cout << "Type: " << (fs::is_directory(p) ? "directory" : (fs::is_regular_file(p) ? "file" : "other")) << "\n";
        std::cout << "Last modified: ";
        auto ftime = fs::last_write_time(p);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now()
            + std::chrono::system_clock::now()
        );
        std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
        std::cout << std::put_time(std::localtime(&cftime), "%F %T") << "\n";
        std::cout << "Permissions: ";
        print_permissions(st.permissions());
        std::cout << "\n";
    } catch (std::exception &e) {
        std::cout << "info error: " << e.what() << "\n";
    }
}

void Explorer::print_permissions(const fs::perms p) {
    // owner, group, others r/w/x
    std::cout << ((p & fs::perms::owner_read) != fs::perms::none ? 'r' : '-')
              << ((p & fs::perms::owner_write) != fs::perms::none ? 'w' : '-')
              << ((p & fs::perms::owner_exec) != fs::perms::none ? 'x' : '-')
              << ((p & fs::perms::group_read) != fs::perms::none ? 'r' : '-')
              << ((p & fs::perms::group_write) != fs::perms::none ? 'w' : '-')
              << ((p & fs::perms::group_exec) != fs::perms::none ? 'x' : '-')
              << ((p & fs::perms::others_read) != fs::perms::none ? 'r' : '-')
              << ((p & fs::perms::others_write) != fs::perms::none ? 'w' : '-')
              << ((p & fs::perms::others_exec) != fs::perms::none ? 'x' : '-') ;
}

void Explorer::cmd_help(const std::vector<std::string>&) {
    std::cout <<
    "Commands:\n"
    "  ls [-a] [path]        List directory\n"
    "  cd [path]             Change directory\n"
    "  pwd                   Print working directory\n"
    "  cp <src> <dst>        Copy file or directory (recursive)\n"
    "  mv <src> <dst>        Move/rename\n"
    "  rm <path> [-r]        Remove file or dir (use -r for dir)\n"
    "  mkdir <dirname>       Create directory\n"
    "  touch <file>          Create or update file timestamp\n"
    "  chmod <octal> <file>  Set permissions (octal, e.g. 755)\n"
    "  search <pattern> [path]  Search recursively for filenames containing pattern\n"
    "  info <file>           Show file info (size, type, mtime, perms)\n"
    "  clear                 Clear the screen\n"
    "  help                  Show this help\n"
    "  exit, quit            Exit explorer\n";
}

void Explorer::cmd_clear(const std::vector<std::string>&) {
    std::cout << "\033[2J\033[1;1H"; // ANSI clear
}
