print([[
#include "param.h"
#include "lookup.h"
#include "varinfo.h"
#include "instruct.h"
#include "set.h"
#include "expr.h"
#include "iocc.h"
#include "y.tab.h"

void enterkeyw(void) {
]])

for line in io.stdin:lines() do
	if not line:find("^#") then
		local _, _, w1, w2 = line:find("^(%g+)%s+(%g+)$")
		print(string.format([[
			lookup("%s", symkeyw, newsymbol)->sy_value.syv_keywno = %s;
		]], w1, w2))
	end
end

print([[
}
]])

