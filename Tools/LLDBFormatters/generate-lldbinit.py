import os
import sys
import datetime
import shutil

def deprecate_file_if_exists(file_path):
    if os.path.exists(file_path):
        print file_path + " already eixsts, moving to " + file_path + ".old"
        os.rename(file_path, file_path + ".old")

def main(script_location):

    # Derive the reop root from this scripts location
    repo_root = os.path.abspath(script_location + "../../../../")
    if not os.path.exists(repo_root):
        print "Failed to find repo root path " + repo_root
        return

    unity_setup_script = os.path.abspath(script_location + "/../unity_lldb_setup.py")
    if not os.path.exists(unity_setup_script):
        print "Failed to find unity setup script: " + unity_setup_script
        return

    lldb_init_path = os.path.expanduser("~/.lldbinit")
    lldb_user_path = os.path.expanduser("~/unity-lldb")
    unity_setup_script_dest = os.path.join(lldb_user_path, "unity_lldb_setup.py")

    if not os.path.exists(lldb_user_path):
        os.makedirs(lldb_user_path)

    deprecate_file_if_exists(unity_setup_script_dest)
    shutil.copy2(unity_setup_script, unity_setup_script_dest)

    with open(unity_setup_script_dest, 'r+') as file_handle:
        file_data = file_handle.read().replace('<INSERT_REPO_PATH>', repo_root)
        file_handle.seek(0)
        file_handle.write(file_data)

    # Write out new lldbinit
    deprecate_file_if_exists(lldb_init_path)

    with open(lldb_init_path, "w") as file_handle:

        hg_branch = "unable to find branch file"
        with open(os.path.join(repo_root, ".hg", "branch"), 'r') as branch_file:
            hg_branch = branch_file.read().replace('\n', '')

        # metadata
        file_handle.write("# Generated: {} \n".format(datetime.datetime.now().strftime("%I:%M%p, %B %d, %Y")))
        file_handle.write("# Repro: '{}'\n".format(repo_root))
        file_handle.write("# Branch: '{}'\n\n".format(hg_branch))

        file_handle.write("command script import " + unity_setup_script_dest)

        print "Set up script successfully written to " + lldb_init_path

main(sys.argv[0])
