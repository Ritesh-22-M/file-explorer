#ifndef EXPLORER_HPP
#define EXPLORER_HPP

#include <string>
#include <vector>
#include <filesystem>

class Explorer {
public:
    Explorer();
    void run();

private:
    std::string cwd;

    // Commands
    void cmd_ls(const std::vector<std::string>& args);
    void cmd_cd(const std::vector<std::string>& args);
    void cmd_pwd(const std::vector<std::string>& args);
    void cmd_cp(const std::vector<std::string>& args);
    void cmd_mv(const std::vector<std::string>& args);
    void cmd_rm(const std::vector<std::string>& args);
    void cmd_mkdir(const std::vector<std::string>& args);
    void cmd_touch(const std::vector<std::string>& args);
    void cmd_chmod(const std::vector<std::string>& args);
    void cmd_search(const std::vector<std::string>& args);
    void cmd_info(const std::vector<std::string>& args);
    void cmd_help(const std::vector<std::string>& args);
    void cmd_clear(const std::vector<std::string>& args);

    // Helpers
    std::vector<std::string> split(const std::string& s);
    void ls_path(const std::string& path, bool show_hidden = false);
    bool copy_recursive(const std::string& src, const std::string& dst);
    void print_permissions(const std::filesystem::perms p);
};
#endif // EXPLORER_HPP
