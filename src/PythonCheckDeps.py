import sys
import subprocess
import pkg_resources

required = {'pip', 'selenium'}
installed = {pkg.key for pkg in pkg_resources.working_set}
missing = required - installed
if(missing) :
    print(missing)
