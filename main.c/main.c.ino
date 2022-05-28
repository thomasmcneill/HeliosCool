


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

  // For every resource available on the server, we need to create a ResourceNode
  // The ResourceNode links URL and HTTP method to a handler function
  ResourceNode * nodeRoot = new ResourceNode("/", "GET", &handleRoot);
  ResourceNode * node404  = new ResourceNode("", "GET", &handle404);

  // Add the root node to the servers. We can use the same ResourceNode on multiple
  // servers (you could also run multiple HTTPS servers)
  secureServer->registerNode(nodeRoot);
  insecureServer->registerNode(nodeRoot);

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


void handleRoot(HTTPRequest * req, HTTPResponse * res) {
  res->setHeader("Content-Type", "text/html");

  res->println("<!DOCTYPE html>");
 
  res->println("<html>");
  res->println("    <head>");
  res->println("    <style>");
  res->println("                                :root {");
  res->println("                                --h1-text-size: 4em;");
  res->println("                                --h2-text-size: 3em;");
  res->println("                                --h3-text-size: 2em;");
  res->println("                                --h4-text-size: 1em;");
  res->println("                                --control-bg-color: #4CAF50;");
  res->println("                                --input-bg-color:white;");
  res->println("                                --input-border-color: #000000;");
  res->println("                                --control-text-color: #ffffff;");
  res->println("                                --control-hover-color: #ddd;");
  res->println("                                --title-bar-height: 50px;");
  res->println("                                --viewport-start: 60px;");
  res->println("                                --control-margin: 5px;");
  res->println("                                --checkbox-checked-color: #4CAF50;");
  res->println("                                --checkbox-unchecked-color: red;");
  res->println("                                --side-bar-closed-width: 2em;");
  res->println("                                --side-bar-open-width:  250px;");
  res->println("");
  res->println("                                }");
  res->println("                                .jsc_hidden {");
  res->println("                                visibility: hidden;");
  res->println("                                }");
  res->println("                                ");
  res->println("                                .jsc_viewport {");
  res->println("                                transition: margin-left .5s; /* If you want a transition effect */");
  res->println("                                display: none;");
  res->println("                                margin-top: var(--viewport-start);");
  res->println("                                }");
  res->println("                                .jsc_div_table");
  res->println("                                {");
  res->println("                                display: table;");
  res->println("                                }");
  res->println("                                .jsc_div_table_cell");
  res->println("                                {");
  res->println("                                display: table-cell;");
  res->println("                                }");
  res->println("                </style>");
  res->println("               ");
  res->println("                <script>");
  res->println("                var jsc_ObjectsDict=new Object;function jsc_AddObject(id){for(var result=\"\",characters=\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\",charactersLength=characters.length,i=0;i<10;i++)result+=characters.charAt(Math.floor(Math.random()*charactersLength));return result=\"jsc_\"+result,jsc_ObjectsDict[id]=result,result}function jsc_toggle_display_byID(id){var el=document.getElementById(id);if(\"none\"===el.style.display)el.hasAttribute(\"jsc_oldstyle\")?el.style.display=el.getAttribute(\"jsc_display_oldstyle\"):el.style.display=\"block\";else{var oldstyle=el.style.display;el.setAttribute(\"jsc_display_oldstyle\",oldstyle),el.style.display=\"none\"}}function jsc_hide_all_viewports(){for(var divsToHide=document.getElementsByClassName(\"jsc_viewport\"),i=0;i<divsToHide.length;i++){var el=divsToHide[i],oldstyle=el.style.display;el.setAttribute(\"jsc_display_oldstyle\",oldstyle),el.style.display=\"none\"}}function jsc_hide_divbyID(id){var el=document.getElementById(id),oldstyle=el.style.display;el.setAttribute(\"jsc_display_oldstyle\",oldstyle),el.style.display=\"none\"}function jsc_show_divbyID(id){var el=document.getElementById(id);el.hasAttribute(\"jsc_oldstyle\")?el.style.display=el.getAttribute(\"jsc_display_oldstyle\"):el.style.display=\"block\"}function jsc_getAllDescendents(id){var allDescendents=[],t=document.getElementById(id).childNodes;for(let i=0;i<t.length;i++)allDescendents.push(t[i]),t[i].hasChildNodes&&jsc_getAllDescendentsRecurse(t[i],allDescendents);return allDescendents}function jsc_getAllDescendentsRecurse(el,descendants){var children=el.childNodes;for(let i=0;i<children.length;i++)descendants.push(children[i]),children[i].hasChildNodes&&jsc_getAllDescendentsRecurse(children[i],descendants)}");
  res->println("");
  res->println("                class class_Gauge extends HTMLElement{connectedCallback(){var myHTML=\"\n\t\t\t\t<style>\n\t\t\t\t\t#jsc_id {\n\t\t\t\t\t\tfont-size: var(--h4-text-size);\n\t\t\t\t\t\tdisplay: inline-block;\n\t\t\t\t\t\tmargin: var(--control-margin);\n\t\t\t\t\t}\n\t\t\t\t</style>\n                                <canvas id='jsc_id' ></canvas>\n\t\t\t\t\n\t\t\t\n\t\t\t\";this.jsc_id=jsc_AddObject(),myHTML=myHTML.replace(/jsc_id/g,this.jsc_id);var parentElementString='document.getElementById(\"'+this.getAttribute(\"id\")+'\")';myHTML=myHTML.replace(/jsc_parent/g,parentElementString),this.myCurrentValue=0,this.hasAttribute(\"value\")&&(this.myCurrentValue=this.getAttribute(\"value\")),this.myUpdateMS=1e3,this.hasAttribute(\"updatems\")&&(this.myUpdateMS=this.getAttribute(\"updatems\")),this.myName=\"\",this.hasAttribute(\"name\")&&(this.myName=this.getAttribute(\"name\")),this.myBackColor=\"white\",this.hasAttribute(\"backcolor\")&&(this.myBackColor=this.getAttribute(\"backcolor\")),this.myWidth=200,this.hasAttribute(\"width\")&&(this.myWidth=this.getAttribute(\"width\")),this.myHeight=this.myWidth,this.hasAttribute(\"height\")&&(this.myHeight=this.getAttribute(\"height\")),this.myWatch=\"\",this.hasAttribute(\"watch\")&&(this.myWatch=this.getAttribute(\"watch\")),this.myMin=0,this.hasAttribute(\"min\")&&(this.myMin=this.getAttribute(\"min\")),this.myMax=255,this.hasAttribute(\"max\")&&(this.myMax=this.getAttribute(\"max\")),this.myGuage_Range=this.myMax-this.myMin,this.myLowZoneEnd=this.myMin,this.hasAttribute(\"lowzoneend\")&&(this.myLowZoneEnd=this.getAttribute(\"lowzoneend\")),this.myLowZoneColor=\"orange\",this.hasAttribute(\"lowzonecolor\")&&(this.myLowZoneColor=this.getAttribute(\"lowzonecolor\")),this.myHighZoneStart=this.myMax,this.hasAttribute(\"highzonestart\")&&(this.myHighZoneStart=this.getAttribute(\"highzonestart\")),this.myHighZoneColor=\"red\",this.hasAttribute(\"highzonecolor\")&&(this.myHighZoneColor=this.getAttribute(\"highzonecolor\")),this.myNormalColor=\"green\",this.hasAttribute(\"normalcolor\")&&(this.myNormalColor=this.getAttribute(\"normalcolor\"),console.log(this.myNormalColor)),this.LowRange_ArcEnd=this.myLowZoneEnd/this.myGuage_Range*180,this.HighRange_ArcStart=this.myHighZoneStart/this.myGuage_Range*180,this.innerHTML=myHTML,this.drawGuage(),setInterval(function(){this.drawGuage()}.bind(this),this.myUpdateMS)}SetInternvalCallback(id){document.getElementById(id).setAttribute(\"value\",window[this.getAttribute(\"watch\")])}gauge_degreesToRadians(degrees){return degrees*(Math.PI/180)}gauge_radiansToDegrees(radians){return radians*(180/Math.PI)}gauge_drawPieSlice(ctx,centerX,centerY,radius,startAngle,endAngle,color){ctx.fillStyle=color,ctx.beginPath(),ctx.moveTo(centerX,centerY),ctx.arc(centerX,centerY,radius,this.gauge_degreesToRadians(startAngle),this.gauge_degreesToRadians(endAngle)),ctx.closePath(),ctx.fill()}drawGuage(){var canvas=document.getElementById(this.jsc_id),context=canvas.getContext(\"2d\");context.canvas.width=this.myWidth,context.canvas.height=this.myHeight;var centerX=canvas.width/2,centerY=10+canvas.height/2,radius=canvas.height/2-10;context.fillStyle=this.myBackColor,context.fillRect(0,0,canvas.width,canvas.height),this.gauge_drawPieSlice(context,centerX,centerY,radius,180,360,this.myNormalColor),this.gauge_drawPieSlice(context,centerX,centerY,radius,180,180+this.LowRange_ArcEnd,this.myLowZoneColor),this.gauge_drawPieSlice(context,centerX,centerY,radius,180+this.HighRange_ArcStart,360,this.myHighZoneColor),context.beginPath(),context.moveTo(centerX,centerY),context.arc(centerX,centerY,.8*radius,1*Math.PI,2*Math.PI,!1),context.fillStyle=this.myBackColor,context.closePath(),context.fill(),context.fillStyle=\"black\",context.font=\"14px Arial\";var textWidth=context.measureText(this.myMin+\" \").width;context.fillText(this.myMin,radius*(1-.8)+textWidth+10,centerY),context.fillStyle=\"black\",context.font=\"14px Arial\",textWidth=context.measureText(this.myMax+\" \").width,context.fillText(this.myMax,canvas.width-radius*(1-.8)-textWidth-10,centerY),textWidth=context.measureText(this.myCurrentValue).width,context.fillText(this.myCurrentValue,canvas.width/2-textWidth/2,centerY+14),this.myCurrentValue=window[this.getAttribute(\"watch\")];var ArcValue=this.myCurrentValue/this.myMax*180;this.gauge_drawPieSlice(context,centerX,centerY,radius,180+ArcValue,180+ArcValue+1,\"black\");var textHeight=0,fontSize=4,maxHeight=canvas.height/2-20,maxWidth=canvas.width-20;for(fontSize=4;fontSize<100;fontSize++){if(context.font=fontSize+\"px Arial\",textHeight=context.measureText(this.myName).height,textWidth=context.measureText(this.myName+\" \").width,textHeight>maxHeight){fontSize--;break}if(textWidth>maxWidth){fontSize--;break}}context.font=fontSize+\"px Arial\",textWidth=context.measureText(this.myName).width,context.fillText(this.myName,canvas.width/2-textWidth/2,centerY+centerY/2)}}customElements.define(\"needle-gauge\",class_Gauge);");
  res->println("");
  res->println("                class class_BasicButton extends HTMLElement{connectedCallback(){var myHTML=\"\n\t\t\t\t<style>\n\t\t\t\t\t#jsc_id {\n\t\t\t\t\t\tfont-size: var(--h4-text-size);\n\t\t\t\t\t\tbackground-color: var(--control-bg-color);\n\n\t\t\t\t\t\tborder: none;\n\t\t\t\t\t\tcolor: white;\n\t\t\t\t\t\tpadding: 1.2ch 2ch;\n\t\t\t\t\t\ttext-align: center;\n\t\t\t\t\t\ttext-decoration: none;\n\t\t\t\t\t\tdisplay: inline-block;\n\t\t\t\t\t\tmargin: var(--control-margin);\n\n\t\t\t\t\t\tcursor: pointer;\n\t\t\t\t\t}\n\t\t\t\t</style>\n\t\t\t\t<button id='jsc_id' onclick='jsc_onclick'>jsc_text</button>\n\t\t\t\n\t\t\t\";this.jsc_id=jsc_AddObject(),myHTML=(myHTML=(myHTML=myHTML.replace(/jsc_id/g,this.jsc_id)).replace(/jsc_text/g,this.getAttribute(\"text\"))).replace(/jsc_onclick/g,this.getAttribute(\"myonclick\"));var parentElementString=\"document.getElementById('\"+this.getAttribute(\"id\")+\"')\";myHTML=myHTML.replace(/jsc_parent/g,parentElementString),this.innerHTML=myHTML}get text(){return document.getElementById(this.jsc_id)}set text(val){document.getElementById(this.jsc_id).text=val}}customElements.define(\"basic-button\",class_BasicButton);");
  res->println("");
  res->println("                class class_DropDown extends HTMLElement{connectedCallback(){var myHTML='\n\t\t\t<style>\n\t\t\t\t.jsc_id {\n\t\t\t\t\tposition: relative;\n  \t\t\t\t\tdisplay: inline-block;\n\t\t\t\t\tmargin: var(--control-margin);\n\t\t\t\t}\n\t\t\t\t.jsc_id_button_class {\n\n\t\t\t\t\tborder: 2px solid var(--input-border-color);\n\t\t\t\t\tborder-radius: 4px;\n\t\t\t\t\t  \n\t\t\t\t\tbackground-color: var(--input-bg-color);\n\t\t\t\t\tpadding: 1.2ch 2ch 1.2ch 2ch;\n\t\t\t\t\tfont-size: var(--h4-text-size);\n\t\t\t\t\tcursor: pointer;\n\t\t\t\t}\n\t\t\t\t.jsc_id_dropdown {\n\t\t\t\t\tposition: relative;\n\t\t\t\t\tdisplay: inline-block;\n\t\t\t\t}\n\t\t\t\t.jsc_id_dropdown a:hover {\n\t\t\t\t\tbackground-color: var(--control-hover-color);\n\t\t\t\t}\n\n\t\t\t\t.jsc_id_dropdown_content {\n\t\t\t\t\tdisplay: none;\n\t\t\t\t\tposition: absolute;\n\t\t\t\t\tbackground-color: var(--control-bg-color);\n\t\t\t\t\tmin-width: 230px;\n\t\t\t\t\toverflow: auto;\n\t\t\t\t\tborder: 1px solid #ddd;\n\t\t\t\t\tz-index: 1;\n\t\t\t\t\tmax-height: 200px;\n    \t\t\t\toverflow: auto;\n\t\t\t\t}\n\t\t\t\t.jsc_id_dropdown_content a {\n\t\t\t\t\tcolor: black;\n\t\t\t\t\tpadding: 1ch 1ch;\n\t\t\t\t\ttext-decoration: none;\n\t\t\t\t\tdisplay: block;\n\t\t\t\t}\n\n\t\t\t\t#jsc_id_input {\n\t\t\t\t\tbox-sizing: border-box;\n\t\t\t\t\tbackground-image: url(\'searchicon.png\');\n\t\t\t\t\tbackground-position: 14px 12px;\n\t\t\t\t\tbackground-repeat: no-repeat;\n\t\t\t\t\tfont-size: 16px;\n\t\t\t\t\tpadding: 14px 20px 12px 45px;\n\t\t\t\t\tborder: none;\n\t\t\t\t\tborder-bottom: 1px solid #ddd;\n\t\t\t\t  }\n\t\t\t\t  .jsc_id_show {\n\t\t\t\t\t  display: block;\n\t\t\t\t\t}\n\t\t\t</style>\n\n\n\t\t\t<div class=\"jsc_id_dropdown\">\n\t\t\t\t<button onclick=\"jsc_onshowfunc\" id=\"jsc_id_button\" class=\"jsc_id_button_class\">jsc_text<i class=\"fa fa-angle-down\"></i></button>\n\t\t\t\t<div id=\"jsc_id_div_dropdown\" class=\"jsc_id_dropdown_content\">\n\t\t\t\t\t\n\t\t\t\t</div>\n\t\t\t</div>\n\n\t\t\t';this.jsc_id=jsc_AddObject();var onshowfunc=\"document.getElementById('\"+this.getAttribute(\"id\")+\"').showDrop()\";this.myPlaceHolder=\"\",this.hasAttribute(\"placeholder\")&&(this.myPlaceHolder=this.getAttribute(\"placeholder\")),this.mySelectedValue=\"\",this.hasAttribute(\"selectedval\")&&(this.mySelectedValue=this.getAttribute(\"selectedval\"));var text=\"Select Value\";this.myPlaceHolder.length>0&&(text=this.myPlaceHolder),this.mySelectedValue.length>0&&(text=this.mySelectedValue),myHTML=(myHTML=(myHTML=myHTML.replace(/jsc_text/g,text)).replace(/jsc_id/g,this.jsc_id)).replace(/jsc_onshowfunc/g,onshowfunc),this.innerHTML=myHTML,console.log(\"this.mySelectedValue=\"+this.mySelectedValue),this.myMenuItems=window[this.getAttribute(\"items\")],myHTML=(myHTML='\t\t\t\t\t<input type=\"text\" placeholder=\"Search..\" id=\"jsc_id_input\" onkeyup=\"'+(\"document.getElementById('\"+this.getAttribute(\"id\")+\"').buildList()\")+'\">').replace(/jsc_id/g,this.jsc_id),document.getElementById(this.jsc_id+\"_div_dropdown\").innerHTML=myHTML,this.buildList()}showDrop(){document.getElementById(this.jsc_id+\"_div_dropdown\").classList.add(this.jsc_id+\"_show\")}selectValue(text){document.getElementById(this.jsc_id+\"_div_dropdown\").classList.remove(this.jsc_id+\"_show\"),document.getElementById(this.jsc_id+\"_button\").innerHTML=`${text}<i class=\"fa fa-angle-down\">`,this.mySelectedValue=text}buildList(){var match=\"\";\"undefined\"!=document.getElementById(this.jsc_id+\"_input\")&&null!=document.getElementById(this.jsc_id+\"_input\")&&(match=(match=document.getElementById(this.jsc_id+\"_input\").value).toUpperCase());var cntnt=document.getElementById(this.jsc_id+\"_div_dropdown\");const elements=document.getElementsByClassName(this.jsc_id+\"_item\");for(;elements.length>0;)elements[0].remove();var myHTML=\"\";if(this.hasAttribute(\"items\"))for(var outer=0;outer<this.myMenuItems.length;outer++){var text=this.myMenuItems[outer][0],dbid=\"\",css=\"\",func=\"\";this.myMenuItems[outer].length>1&&(dbid=this.myMenuItems[outer][1]),this.myMenuItems[outer].length>2&&(css=this.myMenuItems[outer][2]),this.myMenuItems[outer].length>3&&(func=this.myMenuItems[outer][3]),(dbid=\"\")&&(dbid=text),\"\"==func&&(func=`document.getElementById('${this.getAttribute(\"id\")}').selectValue('${text}')`),(text.toUpperCase().indexOf(match)>-1||0==match.length)&&(text==this.mySelectedValue&&(text+='<i class=\"fa fa-check\"></i>'),myHTML+=`<a style=\"${css=this.jsc_id+\"_item \"+css}\" dbid=\"${dbid}\" onclick=\"${func}\">${text}</a>`)}cntnt.innerHTML=myHTML}setValue(text){this.selectValue(text),document.getElementById(this.jsc_id+\"_div_dropdown\").classList.remove(this.jsc_id+\"_show\")}getValue(){return this.mySelectedValue}}customElements.define(\"drop-down\",class_DropDown);");
  res->println("                ");
  res->println("                class class_TextBox extends HTMLElement{connectedCallback(){var myHTML='\n\t\t\t<style>\n\t\t\t\t#jsc_id {\n\t\t\t\t  width: 100%;\n\t\t\t\t  box-sizing: border-box;\n\t\t\t\t  border: 2px solid var(--input-border-color);\n\t\t\t\t  border-radius: 4px;\n\t\t\t\t  font-size: var(--h4-text-size);\n\t\t\t\t  background-color: var(--input-bg-color);\n\t\t\t\t  padding: 1.2ch 2ch 1.2ch 2ch;\n\t\t\t\t  margin: var(--control-margin);\n\t\t\t\t}\n\t\t\t</style>\n\t\t\t<input type=\"text\" id=\'jsc_id\' name=\"jsc_name\" value=\"jsc_value\" placeholder=\"jsc_placeholder\">\n\t\t\t';this.jsc_id=jsc_AddObject(),console.log(this.jsc_id),myHTML=(myHTML=myHTML.replace(/jsc_id/g,this.jsc_id)).replace(/jsc_name/g,this.getAttribute(\"name\")),myHTML=this.hasAttribute(\"value\")?myHTML.replace(/jsc_value/g,this.getAttribute(\"value\")):myHTML.replace(/jsc_value/g,\"\"),this.hasAttribute(\"uuid\")?this.uuid=!0:this.uuid=!1,myHTML=myHTML.replace(/jsc_placeholder/g,this.getAttribute(\"placeholder\"));var parentElementString=\"document.getElementById('\"+this.getAttribute(\"id\")+\"')\";myHTML=myHTML.replace(/jsc_parent/g,parentElementString),this.innerHTML=myHTML}getValue(){return document.getElementById(this.jsc_id).value}setValue(text){document.getElementById(this.jsc_id).value=text}get text(){return document.getElementById(this.jsc_id).value}set text(value){document.getElementById(this.jsc_id).value=value}}customElements.define(\"text-box\",class_TextBox);");
  res->println("");
  res->println("                class class_SideBar extends HTMLElement{connectedCallback(){var myHTML='\n\t\t\t<style>\n\n\t\t\t\t.jsc_id_button_div {\n\t\t\t\t\twidth: var(--side-bar-closed-width);\n\t\t\t\t\tposition: absolute;\n\t\t\t\t\ttop: 0;\n\t\t\t\t\tleft: 0px;\n\t\t\t\t\tmargin: 0px;\n\t\t\t\t\tpadding: 0px;\n\t\t\t\t\theight: var(--title-bar-height);\n\t\t\t\t}\n\t\t\t\t.jsc_id_openbtn {\n\t\t\t\t\twidth: var(--side-bar-closed-width);\n\t\t\t\t\theight: 100%;\n\t\t\t\t\tfont-size: var(--h4-text-size);\n\t\t\t\t\tcursor: pointer;\n\t\t\t\t\tbackground-color: var(--control-bg-color);\n\t\t\t\t\tcolor: white;\n\t\t\t\t\tborder: none;\n\t\t\t\t\tmargin: 0px;\n\t\t\t\t\ttext-align: left;\n\t\t\t\t}\n\n\t\t\t\t.jsc_id_openbtn:hover {\n\t\t\t\t\tbackground-color: var(--control-hover-color);\n\t\t\t\t  }\n  \n\n\t\t\t\t.jsc_id_sidebar {\n\t\t\t\t\theight: 0%; \n\t\t\t\t\twidth: var(--side-bar-closed-width);\n\t\t\t\t\tposition: absolute; \n\t\t\t\t\tz-index: 1; /* on top */\n\t\t\t\t\ttop: 0;\n\t\t\t\t\tleft: 0;\n\t\t\t\t\tbackground-color: var(--control-bg-color);\n\t\t\t\t\toverflow-x: hidden; /* no scroll */\n\t\t\t\t\toverflow: hidden;\n\t\t\t\t\ttransition: 0.5s; \n\t\t\t\t}\n\t\t\t\t.jsc_id_sidebar a, a:visited {\n\t\t\t\t\tpadding: 8px 8px 8px 32px;\n\t\t\t\t\ttext-decoration: none;\n\t\t\t\t\tfont-size: var(--h4-text-size);\n\t\t\t\t\tcolor: var(--control-text-color);\n\t\t\t\t\tdisplay: block;\n\t\t\t\t\ttransition: 0.3s;\n\t\t\t\t}\n\n\t\t\t\t.jsc_id_sidebar a:hover {\n\t\t\t\t\tbackground-color: var(--control-hover-color);\n\t\t\t\t}\n  \n\n\t\t\t\t.jsc_id_closebtn  {\n\t\t\t\t\ttext-align: right;\n\n\t\t\t\t}\n\n\n\n\t\t\t\t/* smaller screens */\n\t\t\t\t@media screen and (max-height: 450px) {\n\t\t\t\t  .jsc_id_sidebar {padding-top: 15px;}\n\t\t\t\t  .jsc_id_sidebar a {font-size: 18px;}\n\t\t\t\t}\n\t\t\t</style>\n\t\t\t<div class=\"jsc_id_button_div\">\n\t\t\t\t<button id=\"jsc_id_openbtn\" class=\"jsc_id_openbtn\" onclick=\"jsc_parent.open()\"><i class=\"fas fa-bars\"></i></button>\n\t\t\t</div>\n\t\t\t<div id=\"jsc_id\" class=\"jsc_id_sidebar\">\n\t\t\t</div>\n\t\t';this.jsc_id=jsc_AddObject(),myHTML=myHTML.replace(/jsc_id/g,this.jsc_id);var parentElementString=\"document.getElementById('\"+this.getAttribute(\"id\")+\"')\";myHTML=myHTML.replace(/jsc_parent/g,parentElementString),this.innerHTML=myHTML,this.myText=\"\",this.hasAttribute(\"mytext\")&&(this.myText=this.getAttribute(\"mytext\"),document.getElementById(this.jsc_id+\"_openbtn\").innerHTML=this.myText),myHTML=document.getElementById(this.jsc_id).innerHTML,myHTML=(myHTML=(myHTML='<a href=\"javascript:void(0)\" class=\"jsc_id_closebtn\" onclick=\"jsc_parent.close()\">&times;</a>').replace(/jsc_parent/g,parentElementString)).replace(/jsc_id/g,this.jsc_id),this.myMenuItems=\"\";var closefunc=\"document.getElementById('\"+this.getAttribute(\"id\")+\"').close()\";if(this.hasAttribute(\"items\")){this.myMenuItems=window[this.getAttribute(\"items\")];for(var outer=0;outer<this.myMenuItems.length;outer++){var text=this.myMenuItems[outer][0];myHTML=myHTML+'<a href=\"#\" class=\"'+this.myMenuItems[outer][1]+'\" onclick=\"'+this.myMenuItems[outer][2]+\"; \"+closefunc+';\"> '+text+\"</a>\"}}document.getElementById(this.jsc_id).innerHTML=myHTML}open(){document.getElementById(this.jsc_id).style.height=\"100%\",document.getElementById(this.jsc_id).style.width=\"var(--side-bar-open-width)\";for(var divsToHide=document.getElementsByClassName(\"jsc_viewport\"),i=0;i<divsToHide.length;i++){divsToHide[i].style.marginLeft=\"250px\"}}close(){document.getElementById(this.jsc_id).style.height=\"0\",document.getElementById(this.jsc_id).style.width=\"var(-side-bar-closed-width)\";for(var divsToHide=document.getElementsByClassName(\"jsc_viewport\"),i=0;i<divsToHide.length;i++){divsToHide[i].style.marginLeft=\"0px\"}}}customElements.define(\"side-bar\",class_SideBar);");
  res->println("");
  res->println("                class class_TitleBar extends HTMLElement{connectedCallback(){var myHTML='\n\t\t\t<style>\n\n\t\t\t\t.jsc_id_div {\n\t\t\t\t\twidth: 100%;\n\t\t\t\t\tposition: absolute;\n\t\t\t\t\ttop: 0;\n\t\t\t\t\tleft: var(--side-bar-closed-width);\n\t\t\t\t\tmargin: 0px;\n\n\t\t\t\t\tfont-size: var(--h4-text-size);\n\t\t\t\t\theight: var(--title-bar-height);\n\t\t\t\t\tbackground-color: var(--control-bg-color);\n\t\t\t\t\tcolor: var(--control-text-color);\n\t\t\t\t\t\n\n\n\t\t\t\t}\n\n\n\t\t\t</style>\n\t\t\t<div class=\"jsc_id_div\">jsc_text</div>\n\t\t';this.jsc_id=jsc_AddObject(),this.text=this.innerHTML,myHTML=myHTML.replace(/jsc_id/g,this.jsc_id);var parentElementString=\"document.getElementById('\"+this.getAttribute(\"id\")+\"')\";myHTML=myHTML.replace(/jsc_parent/g,parentElementString),this.innerHTML=myHTML,document.getElementById(this.getAttribute(\"id\")).style.padding=\"20px;\",document.getElementById(this.getAttribute(\"id\")).style.margin=\"0px;\",document.getElementById(this.getAttribute(\"id\")).style.display=\"\"}}customElements.define(\"title-bar\",class_TitleBar);");
  res->println("                </script>");
  res->println("                ");
  res->println("   <script>");
  res->println("");
  res->println("                        Gauge1 = 10;");
  res->println("                        setInterval(function() { ");
  res->println("                                Gauge1+=10;");
  res->println("                                if(Gauge1 > 254) ");
  res->println("                                Gauge1=0; ");
  res->println("                                   }");
  res->println("                        , 1000);");
  res->println("                        ");
  res->println("     mySliderMenu = [");
  res->println("       ['Home','fa fa-home','showHome()'],");
  res->println("       ['Customers','fa fa-users','showCustomers()'],");
  res->println("       ['Table','fa fa-users','showTable()'],");
  res->println("       ['Settings','fa fa-cog','showSettings()'],");
  res->println("       ];");
  res->println("     myDropDown = [");
  res->println("       ['DropDown 1',    0,  'fa fa-home', 'showHome()'],");
  res->println("       ['DropDown 2',  1,'fa fa-users',  'showCustomers()'],");
  res->println("       ['DropDown 3',  2,'fa fa-cog',  'showSettings()'],");
  res->println("       ];");
  res->println("");
  res->println("");
  res->println("       ");
  res->println("     myStates = [[\"AL\"],[\"AK\"],[\"AZ\"],[\"AR\"],[\"CA\"],[\"CO\"],[\"CT\"],[\"DE\"],[\"FL\"],[\"GA\"],[\"HI\"],[\"ID\"],[\"IL\"],[\"IN\"],[\"IA\"],[\"KS\"],[\"KY\"],[\"LA\"],[\"ME\"],[\"MD\"],[\"MA\"],[\"MI\"],[\"MN\"],[\"MS\"],[\"MO\"],[\"MT\"],[\"NE\"],[\"NV\"],[\"NH\"],[\"NJ\"],[\"NM\"],[\"NY\"],[\"NC\"],[\"ND\"],[\"OH\"],[\"OK\"],[\"OR\"],[\"PA\"],[\"RI\"],[\"SC\"],[\"SD\"],[\"TN\"],[\"TX\"],[\"UT\"],[\"VT\"],[\"VA\"],[\"WA\"],[\"WV\"],[\"WI\"],[\"WY\"],[\"AS\"],[\"DC\"],[\"FM\"],[\"GU\"],[\"MH\"],[\"MP\"],[\"PW\"],[\"PR\"],[\"VI\"],[\"AE\"],[\"AA\"],[\"AE\"],[\"AE\"],[\"AE\"],[\"AP\"]];");
  res->println("");
  res->println("     function showSettings()");
  res->println("     {");
  res->println("       console.log('showSettings');");
  res->println("     }");
  res->println("     function SetZIP()");
  res->println("     {");
  res->println("       document.getElementById('zip').text='12345-1111';");
  res->println("     }");
  res->println("     function ClickSwitch(val)");
  res->println("     {");
  res->println("       alert(\"Switch Clicked \" + val);");
  res->println("     }");
  res->println("");
  res->println("     ");
  res->println("     /*  Show the main page first */");
  res->println("     window.addEventListener(\"load\", function(){");
  res->println("       jsc_show_divbyID(\"main\");");
  res->println(" ");
  res->println("     });");
  res->println("     ");
  res->println("   </script>");
  res->println("");
  res->println("   ");
  res->println("      </head>");
  res->println("");
  res->println("    <body>");
  res->println("   <side-bar id='navbar' pagediv_id='main' items='mySliderMenu' mytext='<i class=\"fas fa-bars\"></i>' ></side-bar>");
  res->println("   <title-bar id='title-bar'></title-bar>");
  res->println("");
  res->println("");
  res->println("");
  res->println("        <div id=\"main\" class=\"jsc_viewport\">");
  res->println("     <basic-button text='test' myonclick='SetZIP()' ></basic-button>");
  res->println("     <drop-down id='dropdown' items='myDropDown' selectedval='DropDown 1'></drop-down>");
  res->println("     <needle-gauge id='myCanvas' watch='Gauge1' value='0' name='Pressure' backcolor='grey' min='0' max='256' lowzoneend='64' lowzonecolor='orange' highzonestart='192' highzonecolor='red' normalcolor='green' width='200' height='200'></needle-gauge>");
  res->println("");
  res->println("            <h2>Collapsed Sidebar</h2>");
  res->println("            <p>Content...</p>");
  res->println("   </div>");
  res->println("");
  res->println("                ");
  res->println("   <div id=\"view_customers\" class=\"jsc_viewport\" >");
  res->println("     <basic-button text='New Customer' myonclick='showNewCustomers()' ></basic-button>");
  res->println("     ");
  res->println("   </div>");
  res->println("");
  res->println("   ");
  res->println("");
  res->println("   ");
  res->println("   <div id=\"view_newcustomer\" class=\"jsc_viewport\" >");
  res->println("     <text-box id='formCustomerID' name='id' dbCol='id' class='jsc_hidden' uuid='true' value='' ></text-box>");
  res->println("     <text-box id='asdfasdg' name='Display Name' placeholder='Display Name' dbCol='name'></text-box>");
  res->println("     ");
  res->println("   </div>");
  res->println("");
  res->println("");
  res->println("");
  res->println("");
  res->println("   <div id=\"view_table\" class=\"jsc_viewport\" >");
  res->println("     <basic-button text='Get Data' myonclick='GetTable()' ></basic-button>");
  res->println("     ");
  res->println("   </div>");
  res->println(" </body>");
  res->println("</html>");

  res->print("<p>Your server is running for ");
  res->print((int)(millis()/1000), DEC);
  res->println(" seconds.</p>");

  // You can check if you are connected over a secure connection, eg. if you
  // want to use authentication and redirect the user to a secure connection
  // for that
  if (req->isSecure()) {
    res->println("<p>You are connected via <strong>HTTPS</strong>.</p>");
  } else {
    res->println("<p>You are connected via <strong>HTTP</strong>.</p>");
  }

  res->println("</body>");
  res->println("</html>");
}
void handle404(HTTPRequest * req, HTTPResponse * res) {
  req->discardRequestBody();
  res->setStatusCode(404);
  res->setStatusText("Not Found");
  res->setHeader("Content-Type", "text/html");
  res->println("<!DOCTYPE html>");
  res->println("<html>");
  res->println("<head><title>Not Found</title></head>");
  res->println("<body><h1>404 Not Found</h1><p>The requested resource was not found on this server.</p></body>");
  res->println("</html>");
}
