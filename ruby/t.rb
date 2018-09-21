require 'rubygems'
require "postgres"

conn = PGconn.connect("localhost", 5432, '', '', "ccpro_noip_org", "ccpro", "")
res = conn.exec("SELECT id, name FROM catalog_common ORDER BY id");

i = 1
for row in res.to_a
#   if i != row[0].to_i then
      printf "%04d - %s\n", row[0].to_i, row[1].to_a;
#      i = row[0].to_i
#   end
#   i += 1
end
