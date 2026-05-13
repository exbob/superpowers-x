---
name: git-tag
description: MUST use for any git tag workflow—release/shipping, SemVer bump (MAJOR/MINOR/PATCH), choosing the next v-prefixed tag, annotated vs lightweight tags, listing/showing tags, deleting local or remote tags, pushing one tag or all tags, recovering from wrong/missing pushes, or pre-release tags (e.g. rc). Also use when the user writes in Chinese about 发版、打标签、语义化版本、推送标签、删标签、版本号该升多少—do not rely on memory; follow this skill.
---

# Git 标签（git tag）使用规范

## 目标

仅提供 `git tag` 相关规范：何时打标签、如何命名版本、如何创建/查看/删除/推送标签。

## 版本命名

统一使用带 `v` 前缀的语义化版本号：

```text
v<MAJOR>.<MINOR>.<PATCH>
```

- `MAJOR`：不兼容变更
- `MINOR`：向后兼容的新功能
- `PATCH`：向后兼容的缺陷修复

## 何时打新标签

只在需要标记发布版本或里程碑时打标签。建议对应已验证的稳定提交。

常见判断：
- 功能发布：通常递增 `MINOR`（或不兼容时递增 `MAJOR`）
- 缺陷修复发布：通常递增 `PATCH`
- 纯文档/格式/流程改动：通常不单独打新版本标签

## 常用命令

创建附注标签（推荐）：

```bash
git tag -a v1.2.3 -m "release: v1.2.3"
```

创建轻量标签：

```bash
git tag v1.2.3
```

查看标签：

```bash
git tag
git show v1.2.3
```

推送标签到远程：

```bash
git push origin v1.2.3
```

一次推送全部本地标签：

```bash
git push origin --tags
```

删除标签：

```bash
git tag -d v1.2.3
git push origin :refs/tags/v1.2.3
```

## 常见错误

- 标签名不规范（如 `1.2.3`、`release-1`）  
  建议统一为 `vMAJOR.MINOR.PATCH`
- 本地打了标签但忘记推送  
  需要执行 `git push origin <tag>`
- 已发布标签被随意改指向  
  避免重写已共享标签，优先递增下一个版本号
