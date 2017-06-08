package hbase;

import java.util.LinkedList;
import java.util.List;
import java.util.Random;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.KeyValue;
import org.apache.hadoop.hbase.TableName;
import org.apache.hadoop.hbase.client.Connection;
import org.apache.hadoop.hbase.client.ConnectionFactory;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.client.ResultScanner;
import org.apache.hadoop.hbase.client.Scan;
import org.apache.hadoop.hbase.client.Table;
import org.apache.hadoop.hbase.util.Bytes;

public class Stus {
	static String TABLE_NAME = "Students";
	static Configuration conf = HBaseConfiguration.create();
	static List<List<String>> list = new LinkedList<List<String>>();

	public static void main(String[] args) {
		// produceData();
		// put();
		scan(TABLE_NAME);
	}	
	
	// 添加一条数据，通过HTable Put为已存在的表添加数据
	public static void put() {
		try {
			String[] cf = { "", "cf_name", "cf_name", "cf_gender", "cf_birthday", "cf_birthday", "cf_birthday",
					"cf_address", "cf_address", "cf_scores", "cf_scores" };
			String[] col = { "", "last_name", "first_name", "gender", "year", "month", "day", "province", "city", "cc",
					"os" };
			Connection con = ConnectionFactory.createConnection(conf);
			TableName tn = TableName.valueOf(TABLE_NAME);
			Table table = con.getTable(tn);
			for (List<String> row : list) {
				Put put = new Put(Bytes.toBytes(row.get(0)));
				for (int i = 1; i < row.size(); i++) {
					put.addColumn(Bytes.toBytes(cf[i]), Bytes.toBytes(col[i]), Bytes.toBytes(row.get(i)));
					table.put(put);
				}
				System.out.println(row.get(0));
			}
			System.out.println("put success");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	// 通过HTable Scan来获取tableName表的所有数据信息
	@SuppressWarnings("deprecation")
	public static void scan(String tableName) {
		try {
			Connection con = ConnectionFactory.createConnection(conf);
			Table table = con.getTable(TableName.valueOf(tableName));
			Scan scan = new Scan();
			ResultScanner resultScanner = table.getScanner(scan);
			for (Result s : resultScanner) {
				for (KeyValue kv : s.list()) {

					System.out.print(Bytes.toString(kv.getQualifier()) + ":" + Bytes.toString(kv.getValue()) + " | ");
				}
				System.out.println();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	// 随机产生数据
	public static void produceData() {
		String[] gender = { "男", "女" };
		String[] lastName = { "赵", "钱", "孙", "李", "周", "吴", "郑", "王", "冯", "陈" };
		String[] firstName = { "一", "二", "三", "四", "五", "六", "七", "八", "九", "十" };
		String[] province = { "河南省", "山东省", "河北省", "江苏省", "山西省", "浙江省", "广东省", "福建省" };
		String city[] = { "洛阳市", "济南市", "石家庄市", "南京市", "太原市", "杭州市", "广州市", "福州市" };
		Random random = new Random();
		String head = "20140106";
		for (int i = 1000; i < 1300; i++) {
			List<String> row = new LinkedList<String>();
			row.add(head + String.valueOf(i));

			row.add(lastName[random.nextInt(lastName.length)]);
			row.add(firstName[random.nextInt(firstName.length)]);

			row.add(gender[random.nextInt(gender.length)]);

			row.add(String.valueOf(1993 + random.nextInt(4)));
			row.add(String.valueOf(random.nextInt(12) + 1));
			row.add(String.valueOf(random.nextInt(28) + 1));

			int n = random.nextInt(province.length);
			row.add(province[n]);
			row.add(city[n]);

			row.add(String.valueOf(random.nextInt(50) + 50));
			row.add(String.valueOf(random.nextInt(50) + 50));
			list.add(row);
		}
		showData();
	}
	
	public static void showData() {
		for (List<String> li : list) {
			for (String str : li) {
				System.out.print(str +" ");
			}
			System.out.println();
		}
	}
}