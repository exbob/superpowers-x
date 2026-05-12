---
name: executing-plans
description: Use when you have a written implementation plan to execute in a separate session with review checkpoints
---

# Executing Plans

## Overview

Load plan, review critically, execute all tasks, report when complete.

**Announce at start:** "I'm using the executing-plans skill to implement this plan."

**Note:** Tell your human partner that Superpowers works much better with access to subagents. The quality of its work will be significantly higher if run on a platform with subagent support (such as Claude Code or Codex). If subagents are available, use superpowers:subagent-driven-development instead of this skill.

## The Process

### Step 1: Load and Review Plan
1. Read plan file
2. Review critically - identify any questions or concerns about the plan
3. If concerns: Raise them with your human partner before starting
4. If no concerns: Run **Step 1.5 (Isolation gate)**, then create TodoWrite and proceed to Step 2

### Step 1.5: Isolation gate (before implementation)

**Do this before marking any task `in_progress` or changing implementation code.** Review-only work in Step 1 does not require isolation.

1. **Detect:** Apply the same checks as `superpowers:using-git-worktrees` Step 0 (linked worktree vs normal checkout; submodule guard). If already in a linked worktree, say so clearly — do not create another worktree.

2. **Recommend (your opinion):** In a short paragraph, weigh at least:
   - Current branch (especially `main` / `master` — see Remember below)
   - Plan size/risk (many tasks or wide blast radius → isolation usually helps)
   - Working tree cleanliness (unrelated local changes → isolation or cleanup first)
   - Whether the partner already stated a worktree preference in this session (honor it; skip redundant ask if they already chose)

3. **Ask:** If not already isolated and the partner has not already declared preference, ask explicitly whether they want an **isolated worktree** for this plan run. If already isolated, you may skip the question and only confirm the detected state unless they asked for something different.

4. **Act:**
   - Wants isolation and not already in a worktree → announce and use **superpowers:using-git-worktrees**, then continue.
   - Declines or cannot create (e.g. sandbox) → work in the current directory per that skill's fallback; do not block the plan solely for this.

### Step 2: Execute Tasks

For each task:
1. Mark as in_progress
2. Follow each step exactly (plan has bite-sized steps)
3. Run verifications as specified
4. Mark as completed

### Step 3: Complete Development

After all tasks complete and verified:
- Announce: "I'm using the finishing-a-development-branch skill to complete this work."
- **REQUIRED SUB-SKILL:** Use superpowers:finishing-a-development-branch
- Follow that skill to verify tests, present options, execute choice

## When to Stop and Ask for Help

**STOP executing immediately when:**
- Hit a blocker (missing dependency, test fails, instruction unclear)
- Plan has critical gaps preventing starting
- You don't understand an instruction
- Verification fails repeatedly

**Ask for clarification rather than guessing.**

## When to Revisit Earlier Steps

**Return to Review (Step 1) when:**
- Partner updates the plan based on your feedback
- Fundamental approach needs rethinking

**Don't force through blockers** - stop and ask.

## Remember
- Review plan critically first
- Follow plan steps exactly
- Don't skip verifications
- Reference skills when plan says to
- Stop when blocked, don't guess
- Never start implementation on main/master branch without explicit user consent

## Integration

**Required workflow skills:**
- **superpowers:using-git-worktrees** - Ensures isolated workspace (creates one or verifies existing)
- **superpowers:writing-plans** - Creates the plan this skill executes
- **superpowers:finishing-a-development-branch** - Complete development after all tasks
