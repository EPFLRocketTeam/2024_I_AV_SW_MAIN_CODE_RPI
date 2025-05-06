# Developer Setup: Raspberry Pi Cross-Compilation with Docker and Dev Containers
This guide sets up a consistent cross-compilation environment for the Raspberry Pi CM4 project using Docker and VS Code. It supports Linux, macOS, and Windows.

## Prerequisites

Install the following tools on your development machine:

* [Docker](https://www.docker.com/products/docker-desktop)
* [Docker Compose](https://docs.docker.com/compose/)
* [GitHub CLI](https://cli.github.com/)
* [Visual Studio Code (VS Code)](https://code.visualstudio.com/)
* VS Code Extensions:
  * [Dev Containers](https://marketplace.visualstudio.com/items/?itemName=ms-vscode-remote.remote-containers)
  * [C/C++ Extension Pack](https://marketplace.visualstudio.com/items/?itemName=ms-vscode.cpptools-extension-pack)

### Platform-specific Notes

| OS          | Notes                                                                                                |
| ----------- | ---------------------------------------------------------------------------------------------------- |
| **Linux**   | Add your user to the Docker group: `sudo usermod -aG docker $USER`                                   |
| **Windows** | Use Docker Desktop with WSL2. See: [Docker WSL2 guide](https://docs.docker.com/desktop/windows/wsl/) |
| **macOS**   | Docker Desktop should work out of the box. Adjust resource settings if needed.                       |

## Cloning the Repository

This project uses Git submodules, some of which are private. You must authenticate with GitHub CLI before cloning.

### Step 1: Authenticate with GitHub

Before cloning, run:

```bash
gh auth login
```

Follow the prompts:

* Select `GitHub.com`
* Choose `HTTPS`
* When prompted, log in via your browser. Use your EPFL email address.

You can verify the login with:

```bash
gh auth status
```

### Step 2: Clone the Repository with Submodules

Once authenticated:

```bash
git clone --recurse-submodules https://github.com/EPFLRocketTeam/2024_I_AV_SW_MAIN_CODE_RPI.git
cd 2024_I_AV_SW_MAIN_CODE_RPI
```

This will initialize all submodules using your GitHub credentials.

## Building the Docker Image

From the root of the project:

```bash
docker-compose build
```

This sets up the Docker container with the cross-compilation toolchain and sysroot.

## Opening the Project in VS Code

1. Open the project folder in VS Code.
2. Click the button in the bottom-left corner.
3. Select **“Reopen in Container”**.
4. Wait for the container to initialize.
5. If you are prompted to select a kit for the workspace, select **[Unspecified]** and press **Enter**.

## Building the Project

### Option A: Terminal inside the Dev Container

```bash
./scripts/build.sh
```

### Option B: CMake Tools in VS Code

1. Open the **CMake** panel in the left sidebar.
2. Select the **Build** option.
3. Click on the small icon on the left of the build option to build the project.
4. The build should start in the terminal.

## Using Git in the Dev Container

Once the Dev Container is running, you can use Git directly within VS Code as usual. The Git integration in VS Code works seamlessly inside the container.

To use Git, use the **Source Control** panel in the left sidebar to view changes, stage, commit, and push.

If this is your first time using Git in the container, you may need to set your identity:

```bash
git config --global user.name "Your Name"
git config --global user.email "your.email@epfl.ch"
```

VS Code will use the GitHub authentication from your host system. Normally, you won't need to re-authenticate if you have already logged in using `gh auth login`.
