.. _dev-release:

Release Checklist
*****************

Things to update immediately before a release, i.e. from a release candidate branch.

1. README - update at the top and the version history.
2. CMakeLists.txt - change major, minor & patch version at very top.

For each submodule:
    * Update version in setup.py.
    * Update version in docs/source/conf.py in 2 places.
    * Update version history in submodule manual.
    * Generate submodule documentation (html + latexpdf); copy pdf to docs dir.
    * Commit pdf of documentation to each submodule.
    * Update main __init__.py / __version__ number to match.
    * Tag submodule version.
    * Upload submodule manual to website.
    * Upload to pypi.

3. Tag submodule repository versions.
4. Update submodules in bdsim repository.
5. Update version history (including submodule versions) in manual source.
6. Regenerate BDSIM manual and commit new version. Upload to website.
7. Merge release candidate branch back into develop.
8. In develop put back README, CMakeLists.txt to new version.develop.
9. Merge release candidate branch into master then delete.
10. Check all tests complete locally given merge before pushing.
11. Tag master branch for version number.
