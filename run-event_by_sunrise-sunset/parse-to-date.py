import urllib.request
import json
import pprint
import calendar
from datetime import datetime, timedelta

def add_record_to_db(month, day, sunrise, sunset):
    insert = insert_template.format(local_zip, now.year, month, day, sunrise, sunset)

def get_cloud_index():
    url = 'https://api.openweathermap.org/data/2.5/weather?zip=98027,us&APPID=' + api_key
    content = urllib.request.urlopen(url).read()
    data = json.loads(content)

    return int((data['clouds']['all'] + 1) / 40 + 1)

def utc_to_local(utc_dt):
    # get integer timestamp to avoid precision lost
    timestamp = calendar.timegm(utc_dt.timetuple())
    local_dt = datetime.fromtimestamp(timestamp)
    assert utc_dt.resolution >= timedelta(microseconds=1)
    return local_dt.replace(microsecond=utc_dt.microsecond)

def get_srss_online(month, day):
    date = '{0}-{1:02d}-{2:02d}'.format(now.year, month, day)

    url = 'https://api.sunrise-sunset.org/json?lat=47.49638&lng=-121.99090&formatted=0&date=' + date

    content = urllib.request.urlopen(url).read()

    data = json.loads(content)

    if data['status'] == 'OK':
        sunrise = datetime.strptime(data['results']['sunrise'], '%Y-%m-%dT%H:%M:%S+00:00')
        sunrise = utc_to_local(sunrise)
        sunrise_hrs = int(sunrise.strftime('%H%M'))

        sunset = datetime.strptime(data['results']['sunset'], '%Y-%m-%dT%H:%M:%S+00:00')
        sunset = utc_to_local(sunset)
        sunset_hrs = int(sunset.strftime('%H%M'))

        add_record_to_db(month, day, sunrise_hrs, sunset_hrs)
        return {'sunrise':sunrise_hrs, 'sunset':sunset_hrs}
    else:
        raise ValueError('wrong date : ' + date)

def find_srss_for_date(month, day):
    if dates[month][day] is not None and type(dates[month][day]) is dict:
        return dates[month][day]
    else:
        return get_srss_online(month, day)

data_table = """0756 1628  0735 1709  0649 1753  0547 1838  0451 1920  0415 1958  0415 2009  0446 1942  0527 1849  0607 1748  0652 1651  0735 1619
0756 1629  0733 1711  0647 1754  0545 1839  0450 1921  0414 1959  0415 2009  0447 1941  0528 1847  0608 1746  0653 1649  0736 1619
0756 1630  0732 1713  0645 1756  0543 1841  0448 1923  0414 1959  0416 2008  0448 1940  0529 1845  0610 1744  0655 1648  0737 1618
0756 1631  0731 1714  0643 1757  0541 1842  0447 1924  0413 2000  0417 2008  0450 1938  0531 1843  0611 1742  0656 1646  0739 1618
0756 1632  0729 1716  0641 1759  0539 1844  0445 1925  0413 2001  0417 2008  0451 1937  0532 1841  0613 1740  0658 1645  0740 1618
0755 1633  0728 1717  0639 1800  0537 1845  0444 1927  0412 2002  0418 2007  0452 1935  0533 1839  0614 1738  0659 1644  0741 1617
0755 1634  0726 1719  0637 1802  0535 1846  0442 1928  0412 2003  0419 2007  0454 1934  0535 1837  0615 1736  0701 1642  0742 1617
0755 1635  0725 1720  0635 1803  0533 1848  0441 1930  0412 2003  0420 2006  0455 1932  0536 1835  0617 1734  0702 1641  0743 1617
0755 1636  0723 1722  0633 1805  0531 1849  0439 1931  0411 2004  0421 2006  0456 1930  0537 1833  0618 1732  0704 1639  0744 1617
0754 1638  0722 1724  0631 1806  0529 1851  0438 1932  0411 2005  0422 2005  0457 1929  0539 1831  0620 1730  0705 1638  0745 1617
0754 1639  0720 1725  0629 1808  0527 1852  0436 1934  0411 2005  0422 2004  0459 1927  0540 1829  0621 1728  0707 1637  0746 1617
0753 1640  0719 1727  0627 1809  0525 1853  0435 1935  0411 2006  0423 2004  0500 1925  0541 1827  0622 1726  0708 1636  0747 1617
0753 1642  0717 1728  0625 1811  0524 1855  0434 1936  0410 2006  0424 2003  0501 1924  0543 1825  0624 1724  0710 1634  0748 1617
0752 1643  0715 1730  0623 1812  0522 1856  0432 1937  0410 2007  0425 2002  0503 1922  0544 1822  0625 1722  0711 1633  0748 1617
0751 1644  0714 1731  0621 1814  0520 1858  0431 1939  0410 2007  0426 2002  0504 1920  0545 1820  0627 1720  0713 1632  0749 1617
0751 1646  0712 1733  0619 1815  0518 1859  0430 1940  0410 2008  0427 2001  0505 1919  0547 1818  0628 1718  0714 1631  0750 1617
0750 1647  0710 1735  0617 1816  0516 1900  0429 1941  0410 2008  0428 2000  0507 1917  0548 1816  0630 1717  0716 1630  0751 1618
0749 1648  0709 1736  0615 1818  0514 1902  0428 1942  0410 2008  0429 1959  0508 1915  0549 1814  0631 1715  0717 1629  0751 1618
0748 1650  0707 1738  0613 1819  0512 1903  0426 1944  0410 2009  0430 1958  0509 1913  0551 1812  0633 1713  0719 1628  0752 1618
0748 1651  0705 1739  0611 1821  0510 1905  0425 1945  0411 2009  0431 1957  0511 1911  0552 1810  0634 1711  0720 1627  0753 1619
0747 1653  0703 1741  0609 1822  0509 1906  0424 1946  0411 2009  0433 1956  0512 1910  0553 1808  0635 1709  0722 1626  0753 1619
0746 1654  0702 1742  0607 1824  0507 1907  0423 1947  0411 2009  0434 1955  0513 1908  0555 1806  0637 1708  0723 1625  0754 1620
0745 1656  0700 1744  0605 1825  0505 1909  0422 1948  0411 2010  0435 1954  0515 1906  0556 1804  0638 1706  0724 1624  0754 1620
0744 1657  0658 1745  0603 1826  0503 1910  0421 1949  0412 2010  0436 1953  0516 1904  0557 1802  0640 1704  0726 1623  0755 1621
0743 1659  0656 1747  0601 1828  0502 1912  0420 1951  0412 2010  0437 1951  0517 1902  0559 1800  0641 1702  0727 1623  0755 1622
0742 1700  0654 1748  0559 1829  0500 1913  0419 1952  0412 2010  0438 1950  0519 1900  0600 1758  0643 1701  0729 1622  0755 1622
0741 1702  0653 1750  0557 1831  0458 1914  0419 1953  0413 2010  0440 1949  0520 1858  0602 1756  0644 1659  0730 1621  0755 1623
0740 1703  0651 1751  0555 1832  0456 1916  0418 1954  0413 2010  0441 1948  0521 1856  0603 1754  0646 1657  0731 1621  0756 1624
0738 1705             0553 1834  0455 1917  0417 1955  0414 2009  0442 1946  0523 1855  0604 1752  0647 1656  0732 1620  0756 1625
0737 1706             0551 1835  0453 1919  0416 1956  0414 2009  0443 1945  0524 1853  0606 1750  0649 1654  0734 1620  0756 1626
0736 1708             0549 1836             0416 1957             0445 1944  0525 1851             0650 1653             0756 1626"""

insert_template = "insert into sunrise_sunset_times (zip, year, month, day, sunrise, sunset) values({0}, {1}, {2}, {3}, {4}, {5})"
now = datetime.now()
dates = [[None for i in range(32)] for j in range(13)]
local_zip = 98027
api_key = 'e172f5276171db59194c0d462804653e'

pp = pprint.PrettyPrinter(indent=4)

lines = data_table.splitlines()

#pp.pprint(dates)

day = 1
for line in lines:
    month = 1
    days = {}

    while month <= 12:
        date = "{0:02d}-{1:02d}".format(month, day)

        times = line[:9]
        line = line[11:]

        sunrise = int(times[0:4].strip() or '0')
        sunset = int(times[5:].strip() or '0')

        if (sunrise > 0 and sunset > 0):
            add_record_to_db(month, day, sunrise, sunset)

            dates[month][day] = {'sunrise':sunrise, 'sunset':sunset}

        month += 1
    day += 1

srss = find_srss_for_date(now.month, now.day)
pp.pprint(srss)

start_time = 655
stop_time = 2345

cloud_index = get_cloud_index()
time_offset = 10 * cloud_index

if start_time < srss['sunrise']:

    print('------------------- sunrise -------------------')
#       os.system('at {0} runlight'.format(time_on))
#       os.system('at {0} stoplight'.format(time_off))
    print('at {0} runlight'.format(start_time))
    print('at {0} +{1} minutes stoplight'.format(srss['sunrise'], time_offset))

if stop_time > srss['sunset']:

    print('------------------- sunset -------------------')

    print('at {0} -{1} minutes runlight'.format(srss['sunset'], cloud_index))
    print('at {0} stoplight'.format(stop_time))
