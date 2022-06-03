


// TODO: Configure your WiFi here
#define WIFI_SSID "<your ssid goes here>"
#define WIFI_PSK  "<your pre-shared key goes here>"


// We will use wifi
#include <WiFi.h>



// Includes for the server
// Note: We include HTTPServer and HTTPSServer
#include <HTTPSServer.hpp>
#include <HTTPServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
// The HTTPS Server comes in a separate namespace. For easier use, include it here.
using namespace httpsserver;

// Create an SSL certificate object from the files included above
SSLCert *cert;

HTTPSServer *secureServer;
HTTPServer *insecureServer; 

// Declare some handler functions for the various URLs on the server
void handleRoot(HTTPRequest * req, HTTPResponse * res);
void handle404(HTTPRequest * req, HTTPResponse * res);

void setup() {
  // For logging
  Serial.begin(115200);
  
  createSSL();
  ConnectWiFi();
 SetupHTTPd();
  

  xTaskCreatePinnedToCore(serverTask, "httpd", 6144, NULL, 1, NULL, 1);
  
}
void loop()
{
  while(1==1) {
      Serial.println("loop()");
      delay(5000);
  }
}
void createSSL()
{
  Serial.println("Creating certificate...");
   
  cert = new SSLCert();
 
  int createCertResult = createSelfSignedCert(
    *cert,
    KEYSIZE_2048,
    "CN=myesp.local,O=acme,C=US");
   
   
  if (createCertResult != 0) {
    Serial.printf("Error generating certificate");
    return; 
  }
 
  Serial.println("Certificate created");
  
}
void SetupHTTPd()
{
  secureServer = new HTTPSServer(cert);
  insecureServer = new HTTPServer();

  registerindex(secureServer,insecureServer);

  // For every resource available on the server, we need to create a ResourceNode
  // The ResourceNode links URL and HTTP method to a handler function
  //ResourceNode * nodeRoot = new ResourceNode("/", "GET", &handleRoot);
  ResourceNode * node404  = new ResourceNode("", "GET", &handle404);

  registerindex(secureServer,insecureServer);

  
  // We do the same for the default Node
  secureServer->setDefaultNode(node404);
  insecureServer->setDefaultNode(node404);
 
  Serial.println("Starting HTTPS server...");
  secureServer->start();
  Serial.println("Starting HTTP server...");
  insecureServer->start();
  if (secureServer->isRunning() && insecureServer->isRunning()) {
    Serial.println("Servers ready.");
  }
}
void ConnectWiFi()
{
  int counter = 0;
  IPAddress local_ip(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);

  // Connect to WiFi
  
  Serial.println("Setting up WiFi");
  WiFi.setAutoConnect(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    counter++;
    if(counter==20)
    {
      
    
      WiFi.mode(WIFI_AP);
      WiFi.softAP("ESP32", "password");
      WiFi.softAPConfig(local_ip, gateway, subnet);
      delay(100);
      Serial.print("AP IP=");
      Serial.println(WiFi.softAPIP());
      return;
    }
  }
  Serial.print("Connected IP=");
  Serial.println(WiFi.localIP());
  
}
void serverTask(void *params) {
  // We need to call both loop functions here
  while(1==1) {
    secureServer->loop();
    insecureServer->loop();

    // Other code would go here...
    delay(10);
  }
}
/* Created on 6/2/2022 7:36:58 PM by HTML2Arduino */
void registerindex(HTTPSServer *secureServer,HTTPServer *insecureServer) {
     ResourceNode* nodeindex = new ResourceNode("/", "GET", &handleindex);
     secureServer->registerNode(nodeindex);
     insecureServer->registerNode(nodeindex);
}

void handleindex(HTTPRequest * req, HTTPResponse * res) {
    res->println("<html>");
    res->println("    <head>");
    res->println("    <link rel=\"stylesheet\" href=\"./css/jsc.css\">");
    res->println("    <script src=\"./controls/jsc.js\"></script>");
    res->println("    <script src=\"./sql-bridge/sql-bridge.js\"></script>");
    res->println("    <script src=\"./controls/side-bar.js\"></script>");
    res->println("    <script src=\"./controls/title-bar.js\"></script>");
    res->println("    <script src=\"./controls/text-box.js\"></script>");
    res->println("    <script src=\"./controls/basic-button.js\"></script>");
    res->println("    <script src=\"./controls/masked-textbox.js\"></script>");
    res->println("    <script src=\"./controls/table-view.js\"></script>");
    res->println("    <script src=\"./controls/drop-down.js\"></script>");
    res->println("    <script src=\"./controls/basic-switch.js\"></script>");
    res->println("                <script src=\"./controls/gauge.js\"></script>");
    res->println("");
    res->println("    <script src=\"./javascript/home.js\"></script>");
    res->println("    <script src=\"./javascript/table.js\"></script>");
    res->println("    <script src=\"./javascript/customers.js\"></script>");
    res->println("    <script src=\"./javascript/newcustomer.js\"></script>");
    res->println("");
    res->println("    <link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css\">");
    res->println("    ");
    res->println("    <script>");
    res->println("");
    res->println("                        Gauge1 = 10;");
    res->println("                        setInterval(function() { ");
    res->println("                                Gauge1+=10;");
    res->println("                                if(Gauge1 > 254) ");
    res->println("                                Gauge1=0; ");
    res->println("                                   }");
    res->println("                        , 1000);");
    res->println("                        ");
    res->println("      mySliderMenu = [");
    res->println("        ['Home','fa fa-home','showHome()'],");
    res->println("        ['Customers','fa fa-users','showCustomers()'],");
    res->println("        ['Table','fa fa-users','showTable()'],");
    res->println("        ['Settings','fa fa-cog','showSettings()'],");
    res->println("        ];");
    res->println("      myDropDown = [");
    res->println("        ['DropDown 1',    0,  'fa fa-home', 'showHome()'],");
    res->println("        ['DropDown 2',  1,'fa fa-users',  'showCustomers()'],");
    res->println("        ['DropDown 3',  2,'fa fa-cog',  'showSettings()'],");
    res->println("        ];");
    res->println("");
    res->println("");
    res->println("        ");
    res->println("      myStates = [[\"AL\"],[\"AK\"],[\"AZ\"],[\"AR\"],[\"CA\"],[\"CO\"],[\"CT\"],[\"DE\"],[\"FL\"],[\"GA\"],[\"HI\"],[\"ID\"],[\"IL\"],[\"IN\"],[\"IA\"],[\"KS\"],[\"KY\"],[\"LA\"],[\"ME\"],[\"MD\"],[\"MA\"],[\"MI\"],[\"MN\"],[\"MS\"],[\"MO\"],[\"MT\"],[\"NE\"],[\"NV\"],[\"NH\"],[\"NJ\"],[\"NM\"],[\"NY\"],[\"NC\"],[\"ND\"],[\"OH\"],[\"OK\"],[\"OR\"],[\"PA\"],[\"RI\"],[\"SC\"],[\"SD\"],[\"TN\"],[\"TX\"],[\"UT\"],[\"VT\"],[\"VA\"],[\"WA\"],[\"WV\"],[\"WI\"],[\"WY\"],[\"AS\"],[\"DC\"],[\"FM\"],[\"GU\"],[\"MH\"],[\"MP\"],[\"PW\"],[\"PR\"],[\"VI\"],[\"AE\"],[\"AA\"],[\"AE\"],[\"AE\"],[\"AE\"],[\"AP\"]];");
    res->println("");
    res->println("      function showSettings()");
    res->println("      {");
    res->println("        console.log('showSettings');");
    res->println("      }");
    res->println("      function SetZIP()");
    res->println("      {");
    res->println("        document.getElementById('zip').text='12345-1111';");
    res->println("      }");
    res->println("      function ClickSwitch(val)");
    res->println("      {");
    res->println("        alert(\"Switch Clicked \" + val);");
    res->println("      }");
    res->println("");
    res->println("      var SQLBridge  = new class_SQLBridge('/sql-bridge/','12345');");
    res->println("");
    res->println("      /*  Show the main page first */");
    res->println("      window.addEventListener(\"load\", function(){");
    res->println("        jsc_show_divbyID(\"main\");");
    res->println("  ");
    res->println("      });");
    res->println("      ");
    res->println("    </script>");
    res->println("");
    res->println("    ");
    res->println("      </head>");
    res->println("");
    res->println("    <body>");
    res->println("    <side-bar id='navbar' pagediv_id='main' items='mySliderMenu' mytext='<i class=\"fas fa-bars\"></i>' ></side-bar>");
    res->println("    <title-bar id='title-bar'></title-bar>");
    res->println("");
    res->println("");
    res->println("");
    res->println("        <div id=\"main\" class=\"jsc_viewport\">");
    res->println("      <basic-button text='test' myonclick='SetZIP()' ></basic-button>");
    res->println("      <masked-textbox id='phone' format='(000)000-0000'></masked-textbox>");
    res->println("      <masked-textbox id='zip' format='00000-0000' label='Zip Code'></masked-textbox>");
    res->println("      <drop-down id='dropdown' items='myDropDown' selectedval='DropDown 1'></drop-down>");
    res->println("      <basic-switch id='switch' round=1 myonclick=\"ClickSwitch\"></basic-switch>");
    res->println("            <h2>Collapsed Sidebar</h2>");
    res->println("            <p>Content...</p>");
    res->println("    </div>");
    res->println("");
    res->println("                <needle-gauge id='myCanvas' watch='Gauge1' value='0' name='Pressure' backcolor='grey' min='0' max='256' lowzoneend='64' lowzonecolor='orange' highzonestart='192' highzonecolor='red' normalcolor='green' width='200' height='200'></needle-gauge>");
    res->println("");
    res->println("    <div id=\"view_customers\" class=\"jsc_viewport\" >");
    res->println("      <basic-button text='New Customer' myonclick='showNewCustomers()' ></basic-button>");
    res->println("      <basic-button text='Load Customer' myonclick='loadCustomer()' ></basic-button>");
    res->println("");
    res->println("      <table-view id='customertable' pagebreak='10' uniquecolumn='ID' sortcolumn='0' rowdoubleclick='loadCustomer'></table-view>");
    res->println("    </div>");
    res->println("");
    res->println("    ");
    res->println("");
    res->println("    ");
    res->println("    <div id=\"view_newcustomer\" class=\"jsc_viewport\" >");
    res->println("      <text-box id='formCustomerID' name='id' dbCol='id' class='jsc_hidden' uuid='true' value='' ></text-box>");
    res->println("      <text-box id='asdfasdg' name='Display Name' placeholder='Display Name' dbCol='name'></text-box>");
    res->println("      <text-box name='firstname' placeholder='First Name' dbCol='firstname'></text-box>");
    res->println("      <text-box name='lastname' placeholder='Last Name' dbCol='lastname'></text-box>");
    res->println("      <text-box name='address' placeholder='Address' dbCol='address'></text-box>");
    res->println("      <text-box name='city' placeholder='City' dbCol='city'></text-box>");
    res->println("");
    res->println("");
    res->println("      <div class=\"div_table\">");
    res->println("        <div class=\"div_table_cell\">");
    res->println("          <text-box name='zip' placeholder='Zip' dbCol='zip'></text-box>");
    res->println("        </div>");
    res->println("        <div class=\"div_table_cell\">");
    res->println("          <drop-down id='statedropdown' items='myStates' selectedval='' placeholder='State' dbCol='state'></drop-down>");
    res->println("        </div>");
    res->println("      </div>");
    res->println("      <basic-button text='Save' myonclick='SaveNewCustomer()' ></basic-button>");
    res->println("    </div>");
    res->println("");
    res->println("");
    res->println("");
    res->println("");
    res->println("    <div id=\"view_table\" class=\"jsc_viewport\" >");
    res->println("      <basic-button text='Get Data' myonclick='GetTable()' ></basic-button>");
    res->println("      ");
    res->println("    </div>");
    res->println("  </body>");
    res->println("</html>");
}
