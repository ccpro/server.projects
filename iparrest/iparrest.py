#!/usr/bin/env python

import re
import os, sys, stat
import pwd, grp
from datetime import datetime
import time
import syslog

# global variables
debug = False
root = True

reject_number = 1000
ed_blocked = False
black_list = {}
white_list = {}
cfg = { 'ipfw':'/sbin/ipfw', 'if':'re0'}

def print_out(s):
#    global root, debug

    if s is None:
        s = ''
    elif not isinstance(s, str):
        s = str(s)

    print('root : ' + str(root))
    print('debug : ' + str(debug))
    if root and not debug:
        syslog.syslog(syslog.LOG_PID, s)
    else:
        print(s)

def update_fw_file():
    global root, debug

    time.sleep(1)

    try:
        if root and not debug:
            list = os.popen('/sbin/ipfw list').read()
            fw_list = '/tmp/fw_list'
        else:
            list = os.popen('cat ipfw_list').read()
            fw_list = './fw_list'

        f = open(fw_list, 'w')
        for l in list.split('\n'):
            f.write(l + os.linesep)
        f.close()
    except Exception as err:
        print_out("error in 'update_fw_file' : " + err)
    except:
        print_out("unexpected error in 'update_fw_file' : " + sys.exc_info()[0])

    try:
        if root and not debug:    
            uid = pwd.getpwnam("www").pw_uid
            gid = grp.getgrnam("www").gr_gid
            os.chown(fw_list, uid, gid)
    except Exception as err:
        print_out("error in 'update_fw_file' : " + err)
    except:
        print_out("unexpected error in 'update_fw_file' : " + sys.exc_info()[0])


def add_white(ip):
    global white_list
    
    unblock_addr(ip) 
    white_list[ip] = 1       
    
def show_list():
    global white_list, black_list, ed_blocked, xbox_blocked, xbox_blocked, tv_blocked
    
    print_out('white list:')
    print_out(white_list)
    print_out('black list:')
    print_out(black_list)
    print_out('ed blocked:')
    print_out(ed_blocked)
    print_out('xbox blocked:')
    print_out(xbox_blocked)
    print_out('ghome blocked:')
    print_out(ghome_blocked)
    print_out('tv blocked:')
    print_out(tv_blocked)
    
def block_ed(block):
    if block:
        deny_rule('10.1.1.68', 150)
        deny_rule('10.1.1.6', 151)
    else:
        remove_rule(150)
        remove_rule(151)
    
def block_xbox(block):
    if block:
        deny_rule('10.1.1.90', 152)
        deny_rule('10.1.1.14', 153)
    else:
        remove_rule(152)
        remove_rule(153)
    
def block_ghome(block):
    if block:
        deny_rule('10.1.1.75', 154)
        deny_rule('10.1.1.76', 155)
    else:
        remove_rule(154)
        remove_rule(155)
    

def block_tv(block):
    if block:
        deny_rule('10.1.1.128', 156)
    else:
        remove_rule(156)
    
def clear_list():
    global black_list
    
    for ip, rule_number in black_list:
        remove_rule(rule_number)
    black_list = {}
    
def block_ip(ip, block):
    global black_list

    if block and ip not in black_list.keys():
        block_addr(ip)
    else:
        unblock_addr(ip)
        del black_list[ip]
        
def reset_fw():
    global ed_blocked, xbox_blocked, ghome_blocked, tv_blocked, reject_number, root, debug

    head = """
#!/bin/sh
   
#/sbin/natd -config /etc/natd.conf
   
/sbin/ipfw -f flush
/sbin/ipfw add 100 divert natd all from any to any via $cfg->{iface}\n
#/sbin/ipfw add allow tcp from any to any 18081 in\n
#/sbin/ipfw add fwd 10.1.1.103, 18081 tcp from any to any 18081 in\n
\n"""
    tail = """
/sbin/ipfw add 50000 allow gre from any to any keep-state
/sbin/ipfw add 50100 pass all from any to any\n"""

    if ed_blocked:
        head += '/sbin/ipfw add 150 deny ip from 10.1.1.68 to any\n'
        head += '/sbin/ipfw add 151 deny ip from 10.1.1.6 to any\n'

    if xbox_blocked:
        head += '/sbin/ipfw add 152 deny ip from 10.1.1.90 to any\n'
        head += '/sbin/ipfw add 153 deny ip from 10.1.1.14 to any\n'

    if ghome_blocked:
        head += '/sbin/ipfw add 154 deny ip from 10.1.1.75 to any\n'
        head += '/sbin/ipfw add 155 deny ip from 10.1.1.76 to any\n'

    if tv_blocked:
        head += '/sbin/ipfw add 156 deny ip from 10.1.1.128 to any\n'

    if root and not debug:
        tmp_natd = '/tmp/natd.sh'
        natd = '/usr/local/etc/rc.d/fw.sh'
    else:
        tmp_natd = './natd.sh'
        natd = './fw.sh'

    try:
        f = open(tmp_natd, 'w')
        f.write(head)
        f.write(tail)
        f.close()
    except Exception as err:
        print_out("error in 'reset_fw' : " + err)
    except:
        print_out("unexpected error in 'reset_fw' : " + sys.exc_info()[0])

    reject_number = 1000
    
    now = datetime.now()
    timestamp = str(datetime.timestamp(now))
    
    cmd = "mv {0} {1}.{2}~".format(natd, natd, timestamp)

    try:
        if root and not debug:
            os.system(cmd)
        else:
            print_out(cmd)

        cmd = 'mv {0} {1}'.format(tmp_natd, natd)
        if root and not debug:
            os.system(cmd)
        else:
            print_out(cmd)

        os.chmod(natd, stat.stat.S_ISUID)
    
        cmd = natd
        if root and not debug:
            os.system(cmd)
        else:
            print_out(cmd)
    except Exception as err:
        print_out("error in 'reset_fw' : " + err)
    except:
        print_out("unexpected error in 'reset_fw' : " + sys.exc_info()[0])

def handle_cmd(cmd, ip):
    print_out("iparrest command found : " + cmd + " " + ip)
    
    if 'add-white':
        add_white(ip)
        update_fw_file()
    elif 'show-list':
        show_list()
        update_fw_file()
    elif 'block-ed':
        block_ed(True)
        update_fw_file()
    elif 'unblock-ed':
        block_ed(False)
        update_fw_file()
    elif 'block-xbox':
        block_xbox(True)
        update_fw_file()
    elif 'unblock-xbox':
        block_xbox(False)
        update_fw_file()
    elif 'block-ghome':
        block_ghome(True)
        update_fw_file()
    elif 'unblock-ghome':
        block_ghome(False)
        update_fw_file()
    elif 'block-tv':
        block_tv(True)
        update_fw_file()
    elif 'unblock-tv':
        block_tv(False)
        update_fw_file()
    elif 'clear-list':
        clear_list()
        update_fw_file()
    elif 'unblock-ip':
        block_ip(ip, False)
        update_fw_file()
    elif 'block-ip':
        block_ip(ip, True)
        update_fw_file()
    elif 'reset-fw':
        reset_fw()
        update_fw_file()
    else:
        print_out('unknown command : ' + cmd)

def unblock_addr(ip):
    global remove_rule, black_list, cfg

    if ip in black_list.keys():
        rule_number = black_list[ip]
        cmd = remove_rule.format(cfg['ipfw'], rule_number)
        print_out(cmd)
        del black_list[ip]

def remove_rule(rule_number):
    global remove_rule, cfg

    cmd = remove_rule.format(cfg['ipfw'], rule_number)
    print_out(cmd)

def block_addr(ip):
    global reject_rule, reject_number, cfg, black_list

    cmd = reject_rule.format(cfg['ipfw'], ++reject_number, ip, cfg['if'])
    print_out(cmd)
    
    black_list[ip] = reject_number
    
def deny_addr(ip, rule_number):
    global deny_rule, cfg

    cmd = deny_rule.format(cfg['ipfw'], rule_number, ip)
    print_out(cmd)
    
def handle_attack(ip):

    global black_list

    if ip not in black_list.keys():
        print_out("attack found from : " + ip)
        block_addr(ip)

def parse_number(line):
    parts = line.split(' ')
    if parts is not None and len(parts) > 1:
        return int(parts[0]), parts[4]
        
    return 0, ''

def read_fw_list():

    global reject_number, black_list, root, bebug

    try:
        if root and not debug:
            list = os.popen('/sbin/ipfw list').read()
        else:
            list = os.popen('cat ipfw_list').read()
    except Exception as err:
        print_out("error in 'read_fw_list' : " + err)
    except:
        print_out("unexpected error in 'read_fw_list' : " + sys.exc_info()[0])

    for l in list.split('\n'):

        if 'deny' in l :
            num, ip = parse_number(l)
            if 150 == num or 151 == num:
                ed_blocked = True
            elif 152 == num or 153 == num:
                xbox_blocked = True
            elif 154 == num or 155 == num:
                ghome_blocked = True
            elif 156 == num:
                tv_blocked = True

        elif 'reject' in l:
            num, ip = parse_number(l)
            if num > 0:
                black_list[ip] = num
                if num > reject_number:
                    reject_number = num

#    print_out(black_list)


reject_rule = '{0} add {1} reject all from {2} to any via {3}';
deny_rule = '{0} add {1} deny ip from {2} to any'
remove_rule = '{0} delete {1}'

cmd_regex = re.compile(r'.*GET /iparrest/(.*)/(.*) HTTP.*')
cmd_error = re.compile('^(.*)\s-\s-\s\[(.*)\].*\s40[450]\s.*?$')
local_ip = re.compile('^10\.1\..*$')

print_out('start')

if __name__=='__main__':
    for i in range(1, len(sys.argv)):
        if sys.argv[i][:2] == "-d":
            debug = True
            print_out('debug mode on')
            
    if os.geteuid() > 0:
        print(' getuid ' + str(os.geteuid()))
        root = False
        print(' root defined : ' + str(root))

read_fw_list()

if debug:
    print_out("ed blocked : " + str(ed_blocked))
    print_out("reject max number : " + str(reject_number))

for line in sys.stdin:

    line = line.rstrip()
    if root and not debug:
        print(line)
    
    cmd = cmd_regex.match(line)
    print_out('cmd : ' + str(cmd) + ' : ' + line)
    if cmd:
        command, ip = cmd.groups()
        print_out('command found ' + command + ' : ' + ip)
        handle_cmd(command, ip)
        continue
        
    err = cmd_error.match(line)
    if err :
        ip = local_ip.match(err.groups()[0])
        if ip :
            continue
        else :
            if 'delby' in line or '/lj/' in line:
                continue
            handle_attack(err.groups()[0])
