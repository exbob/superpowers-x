# superpowers-x

适用于**交叉编译**方式开发**嵌入式 Linux**软件的 AI 编程工作流。

在 [Superpowers](https://github.com/obra/superpowers) 基础上整理的，包括 `hooks`、`skills`、`rules`、`subagents` 等，在 Cursor 里使用。本仓库是工作流与约束沉淀，不是业务应用仓库。

## 安装与使用

### 一键安装到业务项目

在已创建的业务项目根目录下执行：

```bash
TOOLKIT_SRC="/tmp/superpowers-x-toolkit-src"
rm -rf "$TOOLKIT_SRC" && \
git clone git@github.com:exbob/my-vibe-coding-toolkits.git "$TOOLKIT_SRC" && \
mkdir -p .cursor && \
cp -rf "$TOOLKIT_SRC"/agents "$TOOLKIT_SRC"/hooks "$TOOLKIT_SRC"/skills "$TOOLKIT_SRC"/rules .cursor/ && \
cp -f "$TOOLKIT_SRC"/clang-format/c/.clang-format . && \
cp -f "$TOOLKIT_SRC"/AGENTS-template.md ./AGENTS.md && \
mv -f .cursor/hooks/hooks.json .cursor/hooks.json
```

注意：

- 若目标目录不存在会自动创建（如 `.cursor`）。
- 若存在同名文件或目录会直接覆盖（含 `.cursor/` 下内容与根目录 `.clang-format`、`AGENTS.md`）。
- 升级工具库后再次执行会覆盖 `AGENTS.md`；若你已深度定制，请先备份或自行合并模板变更。

### 安装后

根据实际项目编辑项目根目录的 **`AGENTS.md`**，说明项目的开发和运行环境，编译和部署的方法，以及开发过程的约束，让 AI Agent 能够准确地理解并执行。

### 嵌入式 Linux（交叉编译）推荐工作流

以下顺序以本仓库技能为主干；宿主机/目标机验证分层、禁止在宿主机直接跑目标架构二进制等见 **`.cursor/rules/cross-compilation-workflow.mdc`**，并与 `AGENTS.md` 中的 Tier A/B 描述保持一致。

1. **初始工程**
   - 按项目形态二选一：用 **`writing-linux-c-small-app`** 或 **`writing-linux-c-complex-app`** 生成初始 CMake/C 工程模板。

2. **规格（spec）**
   - 用 **`brainstorming`** 收敛需求并产出 spec 文档（如 `docs/superpowers/specs/` 等团队约定路径）。
   - 派子代理做 **spec 审核**（规格完整、与交叉编译/部署边界一致）。

3. **计划（plan）**
   - 用 **`writing-plans`** 生成可执行 plan 文档（如 `docs/superpowers/plans/`）。
   - 派子代理做 **plan 审核**（任务拆分、验收命令、与 Tier A/B 一致）。

4. **执行 plan（二选一或组合）**
   - **`executing-plans`**：适合在当前会话内连续推进、任务耦合较高的计划；也可按批量阶段执行并插入**人工检查点**。
   - **`subagent-driven-development`**：适合多个独立子任务，按任务分派子代理；建议 **两阶段审核**——先规格/计划合规，再代码质量。

5. **实现阶段：TDD**
   - 用 **`test-driven-development`** 严格红—绿—重构：先写失败测试，再写最少实现使其通过。

6. **代码评审**
   - **`requesting-code-review`** / **`receiving-code-review`**：发起或响应评审，处理关键问题后再合并。

7. **收尾**
   - **`finishing-a-development-branch`**：跑完约定验证（含 **`verification-before-completion`**：无证据不声称完成），再选择本地合并 / 提 PR / 保留分支 / 丢弃，并清理工作区。

排障与日常质量：出现异常时优先 **`systematic-debugging`**；提交信息可用 **`git-commit`**（Conventional Commits）；需要并行分派时用 **`dispatching-parallel-agents`**。

一句话总结：**先模板与规格计划，再隔离执行；交叉编译下分清宿主机构建与目标机验证；TDD 与强验证支撑交付；评审与分支收尾闭环。**

## 文件说明

下面按本仓库源目录说明各块职责。

### rules

Cursor 规则（`.mdc`），安装到 `.cursor/rules/`。与 Superpowers 及本仓库嵌入式场景相关的条目包括：

- **`cross-compilation-workflow.mdc`**（`alwaysApply`）：交叉编译工作流；宿主机 / 目标机分工；Tier A / Tier B 验证；默认禁止在宿主机直接执行目标架构二进制（除非明确使用 qemu 等仿真路径）。
- **`behavioral-guidelines.mdc`**：与 Agent 协作时的行为约束（先想清楚再写、最小改动、完成前需可验证证据等）。
- **`c-coding-style.mdc`**：C 源码风格约定（与根目录 `.clang-format` 配套）。
- **`shell-script-style.mdc`**：Shell/Bash 脚本风格与 Bash 优先等约定。

### hooks

Cursor **会话启动**钩子，安装后配置在 `.cursor/hooks.json`，可执行脚本在 `.cursor/hooks/`：

- **`hooks.json`**：注册 `sessionStart`，调用 `.cursor/hooks/run-hook.cmd session-start`。
- **`run-hook.cmd`**：跨平台 polyglot 入口；在 Windows 下由 cmd 转调 bash，在 Unix 下由 shell 直接执行同名无扩展脚本，避免 `.sh` 与平台自动探测冲突。
- **`session-start`**：读取 `.cursor/skills/using-superpowers/SKILL.md`，把技能体系入口与调用原则注入会话上下文；若检测到旧版 `~/.config/superpowers/skills` 目录会给出迁移提示。

### skills

每个技能为独立子目录，内含 `SKILL.md`（元数据、触发条件、执行步骤）；部分技能附带 `references/` 等子资源。安装目录为 `.cursor/skills/`。分为如下几类：

**元技能**：

- `using-superpowers`：会话入口——如何发现与调用技能（含优先读技能文件）
- `writing-skills`：新建、编辑或部署前验证技能（`SKILL.md`）本身

**工作流**：

- `brainstorming`：创意与功能改动前的意图澄清、需求与设计发散收敛
- `writing-plans`：在有多步规格或需求、动手写代码之前撰写可执行计划
- `executing-plans`：已有书面实施计划时，在独立会话中带检查点执行
- `subagent-driven-development`：当前会话内按计划将独立任务分派给子代理执行
- `dispatching-parallel-agents`：两个及以上可独立推进任务时的并行分派策略
- `verification-before-completion`：声称完成、修复或通过前，必须先跑验证命令并核对输出
- `test-driven-development`：TDD，在写实现代码前先写失败测试
- `requesting-code-review`：任务完成、重大功能或合并前发起评审以核对需求
- `receiving-code-review`：收到代码评审反馈后，先核验再落实（避免盲从）
- `finishing-a-development-branch`：实现与测试完成后，选择合并、PR、保留或丢弃并清理工作区
- `systematic-debugging`：缺陷、测试失败或异常行为时的系统化排障（先证据与根因）


**工具**：

- `using-git-worktrees`：需要与当前工作区隔离时，用工作树等准备干净开发/执行基线
- `git-commit`：Conventional Commits 风格的提交信息规范与实践
- `git-tag`：发版/打标签、SemVer（含 `v` 前缀）、标注/轻量标签、推送与删除标签等
- `bash-scripting`：编写、编辑、审查或重构 Bash/Shell 脚本及 CI 中的 shell 步骤
- `writing-linux-c-small-app`：Linux 用户态 C 小型工具/CLI（CMake、命令行驱动、无大配置文件、stdout 式日志）
- `writing-linux-c-complex-app`：Linux 用户态 C 复杂应用（CMake、JSON 主配置、zlog、模块化等，含交叉编译场景检查）

### agents

Cursor **Agent 提示模板**（Markdown），安装到 **`.cursor/agents/`**，适合作为只读子代理或专项审核会话的系统提示（与 README 工作流中的 spec / plan 审核步骤配合）：

- **`spec-reviewer.md`**：审核规格文档是否完整、无矛盾、范围合适、可进入计划阶段（含 YAGNI 等检查表思路）。
- **`plan-reviewer.md`**：审核实现计划是否与 spec 对齐、任务拆分是否可执行、是否缺验收步骤等。

使用前将文中占位符（如 `[SPEC_FILE_PATH]`、`[PLAN_FILE_PATH]`）替换为实际路径或在上文粘贴文档内容。

### clang-format

本仓库在 **`clang-format/`** 下提供多套配置，安装命令默认将 **`clang-format/c/.clang-format`** 复制到业务项目**根目录** `.clang-format`，与 **`c-coding-style`** 规则一致。若项目遵循 **Linux 内核**风格，可改为复制 **`clang-format/kernel/.clang-format`**，或保留多套路径由 CMake/脚本按需选用。

## 维护建议

- 新增技能时，遵循现有目录命名风格（短横线、语义清晰）。
- 技能说明优先写「何时使用」，再写「如何执行」。
- 规则文件放 `rules/`，技能放 `skills/`，避免混放。
- 对脚本和规则变更，建议同步更新本 README，确保文档与仓库状态一致。
