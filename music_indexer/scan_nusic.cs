using System;
using System.Collections.Generic;
using System.Linq;

namespace ScanMusic {
    public class Config {
       private static Config _instance;
       public static Config Instance {
          get {
             if (null == _instance) {
                _instance = new Config();
             }
             return _instance;
          }
       }
       
       public List<string> ScanPaths{get; private set;}
       
       public Config() {
          ScanPaths = new List<string>();
       }
       
       public void SetArgs(string[]args) {
           for(int i=0; i<args.Length; ++i) {
              switch (args[i]) {
                 case "-p":
                     ScanPaths.Add(args[++i]);
                     break;
              }
           }
       }
    }

    static class Program { 
    
        private static Config _cfg = Config.Instance;
    
        static void Main(string[]args) {
        
            _cfg.SetArgs(args);
        
            DoScan();
        }
    
        public static void DoScan() {
           foreach (var path in _cfg.ScanPaths) {
              Console.WriteLine(string.Format("going to scan folder {0}", path));
           }
        }
    }
}