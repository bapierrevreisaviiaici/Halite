import os
import sys
import platform
from subprocess import call

def view_replay(browser: str, log: str):
    on_windows = "Windows" in platform.system()

    # Extract only the filename and ensure the output is in visualizer/
    output_filename = os.path.basename(log).replace(".hlt", ".htm")
    path_to_file = os.path.join("visualizer", output_filename)

    if on_windows:
        path_to_file = os.path.join(os.getcwd(), path_to_file)
        if not browser.endswith(".exe"):
            browser += ".exe"

    # Correct path formation based on where the log file is actually stored
    if not log.startswith("replays"):
        log = os.path.join("replays", log)  # Ensure this is correct based on your file organization

    if not os.path.exists(path_to_file):
        # Parse replay file
        if os.path.exists(log):
            with open(log, 'r') as f:
                replay_data = f.read()
        else:
            print(f"Error: Could not find the replay file at {log}")
            return

        # Parse template HTML
        template_path = os.path.join("visualizer", "Visualizer.htm")
        if os.path.exists(template_path):
            with open(template_path, 'r') as f:
                html = f.read()
        else:
            print(f"Error: Could not find the template HTML at {template_path}")
            return

        html = html.replace("FILENAME", os.path.basename(log))
        html = html.replace("REPLAY_DATA", replay_data)

        # Dump replay HTML file
        with open(path_to_file, 'w') as f:
            f.write(html)

    if on_windows:
        # Add quotes if the browser path contains spaces or parentheses
        browser = f'"{browser}"' if any(c in browser for c in ' \t()') else browser
        path_to_file = "file://" + path_to_file

    start_vis_cmd = f"{browser} {path_to_file}"

    if on_windows:
        start_vis_cmd = "start /B " + start_vis_cmd
    else:
        start_vis_cmd += " &"

    with open(os.devnull, "w") as null:
        call(start_vis_cmd, shell=True, stderr=sys.stderr, stdout=null)

def main():
    if len(sys.argv) < 3:
        print("Usage: python vis.py [browser] [log]")
        return
    view_replay(sys.argv[1], sys.argv[2])

if __name__ == "__main__":
    main()
