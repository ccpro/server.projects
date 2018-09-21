#!/usr/local/bin/ruby -w

require 'pathname'
require 'ftools'
require "RMagick"

def buildImageList path
   image_list = []

   Dir.foreach(path) do |image_name|
      image_list.push(File.join(path, image_name)) if /^.*?\.(jpg|jpeg|tiff|tif|gif|png)$/.match(image_name)
   end
               
   image_list
end 

def readDir path

   dir_list = []
   index_time = 0
   index_xml_time = 0

   Dir.foreach(path) do |name|
      if File.directory?(File.join(path,name)) == true && name[0,1] != "."
         dir_list.push(File.join(path,name))
      else 
         if name == ".index" && File.file?(File.join(path,name)) == true
            index_time = File.new(File.join(path,name)).mtime.to_i
         end
         if name == ".index.xml" && File.file?(File.join(path,name)) == true
            index_xml_time = File.new(File.join(path,name)).mtime.to_i
         end
      end
   end

   image_list = buildImageList path

   rebuild_images = false
   image_list.each do |image|
      img_time =  File.new(image).mtime.to_i
      tmp = File.split(image);
      tmb_name = File.join(tmp[0], ".thumbnails", tmp[1])
      if File.exists?(tmb_name)
         tmb_time = File.new(tmb_name).mtime.to_i
         if img_time > tmb_time 
            rebuild_images = true
            break
         end
      end
   end

   if (index_time > 0 && index_time > index_xml_time) || rebuild_images
      puts path

      data = parseFile(File.join(path, ".index"))
      if !data.empty?
        buildXMLIndex(path, data, image_list)
      end
   end

   dir_list.each {|dir| readDir dir}
end

def makeThumbnail fname
   img = Magick::Image::read(fname).first

   scale = 0.
   if img.columns > img.rows
      scale = img.columns/250.
   else
      scale = img.rows/250.
   end

   new_fname = File.split(fname)

   File.makedirs(File.join(new_fname[0], ".thumbnails"))
   command = 'convert -antialias -strip -thumbnail 300 "'+fname+'" "'+File.join(new_fname[0], '.thumbnails', new_fname[1])+'"'
   system command
   File.join(".thumbnails", new_fname[1])
end

def buildXMLIndex path, data, image_list

   xml = File.new(File.join(path, ".index.xml"), "w")
   old_stdout = $stdout
   $stdout = xml

   puts "<?xml version='1.0' encoding='utf-8'?>"
   puts "<!-- created by "+$0+" -->"

   puts "<Data "+(data["cdrip"]? "Source='CDRIP'":"")+" Created='"+Time.now().to_s+"'>"
   data.delete("cdrip")
   desc = ""
   if data.has_key?("description") && data["description"].length
     desc = "\t<Description>\n"+
               "\t<![CDATA[\n"+
               data["description"]+"\n"+
               "\t]]>\n"+
               "\t</Description>"
     data.delete("description")
   end
   
   if data.has_key?("genres") && data["genres"].length
     puts "\t<Genreg>"
     data["genres"].each { |genre| puts "\t\t<Genre ID='"+genre+"' />"}
     puts "\t</Genreg>"
     data.delete("genres")
   end

   data.each_key { |key| puts "\t<"+key+" Value='"+data[key]+"' />"}

   puts desc

   if !image_list.empty? 
      puts "\t<Thumbnails>"
      image_list.each { |image|
         puts "\t\t<Thumbnail Name='"+(makeThumbnail(image))+"' />"
      }
      puts "\t</Thumbnails>"
   end

   puts "</Data>"

   xml.close
   $stdout = old_stdout
end

def parseFile fileName

   data = {}
   data["genres"] = []
   desc_flag = false
   
   File.open(fileName, "r").each do |line|
      line = line.chop
      if desc_flag == true
         data["description"] += line+"\n"
      elsif line.downcase == "description"
           desc_flag = true
           data["description"] = ""
      elsif line == "cdrip"
         data["cdrip"] = true
      elsif (a = /^(.*)=\"?(.*?)\"?$/.match(line))
         data[a[1]] = a[2]
      else
        data["genres"].push(line)
      end
   end

   data
end

ARGV.each { |path| readDir path}
