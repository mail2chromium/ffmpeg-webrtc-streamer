var https = require('https');
var express=require('express');
const app=express();
var fs = require('fs');
var path=require('path');
var url=require('url');
const cors=require("cors");
const corsOptions ={
   origin:'*', 
   credentials:true,            //access-control-allow-credentials:true
   optionSuccessStatus:200,
}
app.use(cors(corsOptions)) // Use this after the variable declaration
var mimeTypes = {
     "html": "text/html",
     "jpeg": "image/jpeg",
     "jpg": "image/jpeg",
     "png": "image/png",
     "js": "text/javascript",
     "css": "text/css"};
var options={
      key: fs.readFileSync("./ssl/selfsigned.key"),
      cert: fs.readFileSync("./ssl/selfsigned.crt"),
}
https.createServer(options,(request, response)=>{
    var pathname = url.parse(request.url).pathname;
    var filename ;
    if(pathname === "/"){
        filename = "index.html";
    }
    else{
      filename = path.join(process.cwd(), pathname);
    }
     
    try{
        fs.accessSync(filename, fs.F_OK);
        var fileStream = fs.createReadStream(filename);
        var mimeType = mimeTypes[path.extname(filename).split(".")[1]];
        response.writeHead(200, {'Content-Type':mimeType});
        fileStream.pipe(response);
    }
    catch(e) {
            console.log('File not exists: ' + filename);
            response.writeHead(404, {'Content-Type': 'text/plain'});
            response.write('404 Not Found\n');
            response.end();
            return;
    }
    return;
    
},app).listen(8084,() => {
  console.log(` server listening on port 8084`)
});
