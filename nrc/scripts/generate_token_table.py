# Generate a token-enum => string lookup table

import re

query = re.compile('(TOKEN_.*),')

in_file = open("../src/lexer/token.h", "r")
data = in_file.read()
in_file.close()

result = query.findall(data)

out = '''
const char* TOKEN_NAMES[] = {
'''

for r in result:
    r = r.split(',')[0]
    out += '\t"' + r + '",\n'

out += "};"

print(out)
