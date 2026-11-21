# Development workflow

## Branching: GitHub Flow

```
main ──●────────●──────────●─────────●── (always builds, protected)
        \      /            \       /
         feat/uart-printf     fix/n6-led
```

1. `main` always builds and passes CI. Nobody pushes to it directly.
2. Start each task on a short-lived branch:
   ```bash
   git switch main && git pull
   git switch -c feat/uart-printf
   ```
3. Commit small, working steps (see Commits below).
4. Push and open a pull request:
   ```bash
   git push -u origin feat/uart-printf
   ```
   Fill in the PR template.
5. Wait for CI to go green. Review your own diff on GitHub, and ask Claude for a review (`/code-review`).
6. **Squash and merge**, then delete the branch.
7. Release: tag `main`, then attach the CI firmware artifacts to the GitHub Release.
   ```bash
   git tag -a v0.2.0 -m "UART printf"
   git push --tags
   ```

Why not Gitflow? Its `develop`, `release/*` and `hotfix/*` branches are built for teams that ship scheduled
releases. For one developer they're overhead with no benefit.

## Commits: Conventional Commits

`<type>(<scope>): <what, imperative, lower case>`

| type | use for | example |
|---|---|---|
| feat | new behaviour | `feat(uart): add printf over VCP` |
| fix | bug fix | `fix(h7s): wait for USB33RDY before using port M` |
| refactor | no behaviour change | `refactor(board): move LED table to const array` |
| test | tests only | `test(ringbuf): cover 16-bit counter wrap` |
| docs | docs only | `docs: add N6 boot switch photo` |
| build / ci | CMake / GitHub Actions | `ci: cache Arm toolchain` |
| chore | everything else | `chore: bump stm32n6 HAL to v1.5.0` |

Put the "why" in the commit body when it isn't obvious.

## Versioning: SemVer

`vMAJOR.MINOR.PATCH`. While below `v1.0.0`, bump MINOR for new features and PATCH for fixes.

## Updating vendor code

```bash
git -C third_party/stm32n6/hal fetch --depth 1 origin tag v1.5.0
git -C third_party/stm32n6/hal checkout v1.5.0
git add third_party/stm32n6/hal
git commit -m "chore: bump stm32n6 HAL to v1.5.0"
```
Do this in its own PR, so a regression can be traced back to it.

## Client work

- One **private** repo per client. Never mix client code into this public portfolio.
- Add `docs/HANDOVER.md`: how to build, flash and test, and the hardware assumptions.
- Agree on deliverables and the acceptance test in writing before you start.
