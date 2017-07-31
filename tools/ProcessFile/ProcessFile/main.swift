//
//  main.swift
//  ProcessFile
//
//  Created by Wild, Chris on 18/03/2015.
//  Copyright (c) 2015 Wild, Chris. All rights reserved.
//

import Foundation
import Cocoa


var colourMap:[CGColor:Int]?
var remap:[Int:Int]?
var startIndex = 1

let path = ("~/Dropbox/Midnight/The Citadel/tiled_wip/" as NSString).expandingTildeInPath



func getRawData(image1: NSImage) -> [[Int]]
{
    
    guard let image = image1.cgImage(forProposedRect: nil, context: nil, hints: nil) else {
        return [[Int]]()
    }

    let pixelsWide = Int(image.width )
    let pixelsHigh = Int(image.height)
    
    let pixelData = image.dataProvider!.data
    let data: UnsafePointer<UInt8> = CFDataGetBytePtr(pixelData)

    
    
    var arr = [[Int]]( repeating: [Int]( repeating: Int(0), count: pixelsWide ), count: pixelsHigh )
    
    var index = 0
    
    
    for y in 0..<pixelsHigh {
        
        index = 4*((Int(pixelsWide) * Int(y)))
        
        for x in 0..<pixelsWide {
            
            
            let r = data[index]
            let g = data[index+1]
            let b = data[index+2]
            let a = data[index+3]
            
            index+=4
            
            let colour = CGColor( red: CGFloat(r)/255.0, green: CGFloat(g)/255.0, blue: CGFloat(b)/255.0, alpha: CGFloat(a)/255.0)
            
            var currentColour = 0
            
            
            if a > 0 {
                if let index = colourMap?[ colour ] {
                    currentColour = index
                } else {
                    currentColour = (colourMap?.count ?? 0 )
                    colourMap?[ colour ] = currentColour
                }
                
                
                if let map = remap {
                    if let index = map[currentColour] {
                        currentColour = index
                    }
                }
                
                arr[y][x] = currentColour + startIndex
            }
            

            
        }
        

    }
    
    
    return arr
    
}


func getRawDataLookup(image1: NSImage) -> [[Int]]
{
    
    guard let image = image1.cgImage(forProposedRect: nil, context: nil, hints: nil) else {
        return [[Int]]()
    }
    
    let pixelsWide = Int(image.width )
    let pixelsHigh = Int(image.height)
    
    let pixelData = image.dataProvider!.data
    let data: UnsafePointer<UInt8> = CFDataGetBytePtr(pixelData)
    
    var arr = [[Int]]( repeating: [Int]( repeating: Int(0), count: pixelsWide ), count: pixelsHigh )
    
    var index = 0
    
    
    for y in 0..<pixelsHigh {
        
        index = 4*((Int(pixelsWide) * Int(y)))
        
        for x in 0..<pixelsWide {
            
            
            let r = data[index]
            let g = data[index+1]
            let b = data[index+2]
            let a = data[index+3]
            
            index+=4
            
            let colour = CGColor( red: CGFloat(r)/255.0, green: CGFloat(g)/255.0, blue: CGFloat(b)/255.0, alpha: CGFloat(a)/255.0)
 
            if a > 0 {
                let currentColour = regions_remap[colour.hashValue2]!
                arr[y][x] = currentColour + startIndex - 1
            }
            
        }
        
        
    }
    
    
    return arr
    
}





func processFile( _ inputFile:String, _ outputFile:String, processor: (NSImage)-> [[Int]] )
{
    
    let expandedPath = ( path + "/" + inputFile as NSString).expandingTildeInPath

    guard let image = NSImage(contentsOfFile: expandedPath ) else {
        return
    }

    let data = getRawData(image1: image)

    let name = outputFile
    var fileOutput = ""

    fileOutput += "<layer name=\"\(name)\" width=\"256\" height=\"256\">\n"
    fileOutput += "<data encoding=\"csv\">\n"
    
    var first=true
    
    for row in data {
        
        if !first {
            fileOutput += ",\n"
        }
        
        first=false
        
        let output = row.map( { String($0) } ).joined(separator: ",")
        

        fileOutput +=  output
    }
    
    fileOutput += "\n</data>\n"
    fileOutput += "</layer>\n"
    
    do {
        let url = (path + outputFile as NSString).expandingTildeInPath
        try fileOutput.write(toFile: url, atomically: false, encoding: String.Encoding.utf8)
        
    }
    catch {}
}


func processFile_ColoursUsed( _ inputFile:String, _ outputFile:String )
{
    
    let expandedPath = ( path + "/" + inputFile as NSString).expandingTildeInPath
    
    guard let image = NSImage(contentsOfFile: expandedPath ) else {
        return
    }
    
    let data = getColoursUsed(image1: image)
    
    var fileOutput = ""
    
    
    for row in data {
        
        let r = Int(row.0.components![1] * 255)
        let g = Int(row.0.components![2] * 255)
        let b = Int(row.0.components![3] * 255)
        
        fileOutput +=  "\(r),\(g),\(b),\(row.1)\n"
    }
    
    
    do {
        let url = (path + outputFile as NSString).expandingTildeInPath
        try fileOutput.write(toFile: url, atomically: false, encoding: String.Encoding.utf8)
        
    }
    catch {}
}

func processFileUnknown( _ inputFile:String, _ outputFile:String, _ rowSize:Int, _ upsideDown:Bool, processor: (UInt16)->String )
{
    
    let expandedPath = ( path + "/" + inputFile as NSString).expandingTildeInPath
    
    guard let data = try? Data(contentsOf: URL( fileURLWithPath: expandedPath), options: .alwaysMapped) else {
        return
    }
    
    var fileOutput = ""
    
    
    data.withUnsafeBytes { (bytes: UnsafePointer<UInt16>)->Void in

        var offset = upsideDown ? (rowSize-1)*rowSize : 0
        
        
        for _ in 0..<rowSize {
            for x in 0..<rowSize {


                if x != 0 {
                    fileOutput += ","
                }

                fileOutput += processor(bytes[ offset + x])
                
                
                //fileOutput +=  "\(String(format:"%01x", v))"
                //fileOutput +=  "\(String(format:"%02x", v))"
                //fileOutput +=  "\(String(format:"%03x", v))"
                ///fileOutput +=  "\(String(format:"%04x", d))"
            }
     
            fileOutput += "\n"
            
            if upsideDown {
                offset -= rowSize
            }else{
                offset += rowSize
            }
            
            
        }
        
        
    }
    
    do {
        let url = (path + outputFile as NSString).expandingTildeInPath
        try fileOutput.write(toFile: url, atomically: false, encoding: String.Encoding.utf8)
        
    }
    catch {}
}







func processFileUnknown_asImage( _ inputFile:String, _ outputFile:String, _ rowSize:Int, _ upsideDown:Bool, processor: (UInt16)->PixelData )
{
    
    let expandedPath = ( path + "/" + inputFile as NSString).expandingTildeInPath
    
    guard let data = try? Data(contentsOf: URL( fileURLWithPath: expandedPath), options: .alwaysMapped) else {
        return
    }
    
    let blankPixel = PixelData(a: 0, r: 0, g: 0, b: 0)
    
    var pixelData = [PixelData](repeating: blankPixel, count: Int(rowSize * rowSize))
    
    data.withUnsafeBytes { (bytes: UnsafePointer<UInt16>)->Void in
        
        var offset = upsideDown ? (rowSize-1)*rowSize : 0
        var output = 0
        
        for _ in 0..<rowSize {
            for x in 0..<rowSize {
                
                pixelData[output] = processor(bytes[ offset + x])

                output += 1
                
            }
            
            if upsideDown {
                offset -= rowSize
            }else{
                offset += rowSize
            }
            
            
        }
        
        
    }

    
     // Update pixels
    let image = imageFromARGB32Bitmap(pixels: pixelData, width: rowSize, height: rowSize)
    let url = (path  + outputFile as NSString ).expandingTildeInPath
    
    if image.pngWrite(to: URL( fileURLWithPath: url) ) {
        print("File saved")
    }
        
 }



processFileUnknown_asImage( "map.dat", "/output/water.png", 512, true ) { data in
    var v = UInt8( (data&0xf000) >> 12)
    v = v & 0x01 // water
    return PixelData( a: 255, r: v*255, g: v*255, b: v*255 )
}

processFileUnknown_asImage( "map.dat", "/output/water_route.png", 512, true ) { data in
    var v = UInt8( (data&0xf000) >> 12)
    v = (v & 0x02) == 2 ? 1 : 0// water route
    return PixelData( a: 255, r: v*255, g: v*255, b: v*255 )
}

processFileUnknown_asImage( "map.dat", "/output/ground_route.png", 512, true ) { data in
    var v = UInt8( (data&0xf000) >> 12)
    v = (v & 0x04) == 4 ? 1 : 0// walk route
    return PixelData( a: 255, r: v*255, g: v*255, b: v*255 )
}


// AB C D
// D = Realm
// AB = Region
// C = terrain
processFileUnknown_asImage( "region.dat", "/output/region.png", 128, false ) { data in
    var v = UInt8( (data&0xff00) >> 8)
    return PixelData( a: 255, r: v, g: v, b: v )
}

processFileUnknown_asImage( "region.dat", "/output/realm.png", 128, false ) { data in
    var v = UInt8( (data&0x000f) )
    return PixelData( a: 255, r: v, g: v*16, b: v*16 )
}

processFileUnknown_asImage( "region.dat", "/output/terrain.png", 128, false ) { data in
    var v = UInt8( (data&0x00f0) >> 4)
    return PixelData( a: 255, r: v, g: v*16, b: v*16 )
}

startIndex = 177
colourMap = [CGColor:Int]()
processFile( "regions.png",             "/output/regions.txt", processor: getRawDataLookup )


startIndex = 2048
colourMap = [CGColor:Int]()
remap = [ 0:15, 1:0, 2:1, 3:3, 4:4, 5:14, 6:5, 7:4, 8:2, 9:13, 10:7, 11:8, 12:6, 13:12, 14:10, 15:9, 16:11  ]
processFile( "realms.png",              "/output/realms.txt", processor: getRawData )


startIndex = 1
colourMap = [CGColor:Int]()
remap = [ 0:0, 1:22, 2:27, 3:6, 4:23  ]
processFile( "cities.png",              "/output/cities.txt", processor: getRawData   )


remap = nil

startIndex = 1
colourMap = [CGColor:Int]()

processFile( "other.png",               "/output/other.txt", processor: getRawData   )
processFile( "layer3.png",              "/output/layer3.txt", processor: getRawData  )
processFile( "layer4.png",              "/output/layer4.txt", processor: getRawData   )
processFile( "layer5.png",              "/output/layer5.txt", processor: getRawData   )
processFile( "layer6.png",              "/output/layer6.txt", processor: getRawData  )

startIndex = 54
colourMap = [CGColor:Int]()
processFile( "border.png",              "/output/border.txt", processor: getRawData  )


startIndex = 144
colourMap = [CGColor:Int]()
processFile( "c_land_route.png",        "/output/land_route.txt", processor: getRawData  )


startIndex = 144
colourMap = [CGColor:Int]()
processFile( "c_water_route.png",       "/output/water_route.txt", processor: getRawData  )

startIndex = 144
colourMap = [CGColor:Int]()
processFile( "c_crossing_route.png",    "/output/crossings.txt", processor: getRawData )

startIndex = 33
colourMap = [CGColor:Int]()

processFile( "t_lamd.png",              "/output/t_land.txt", processor: getRawData  )
processFile( "t_isle.png",              "/output/t_isle.txt", processor: getRawData  )
processFile( "t_lakeland.png",          "/output/t_lakeland.txt", processor: getRawData  )

processFile( "t_plains.png",            "/output/t_plains.txt", processor: getRawData  )
processFile( "t_plains2.png",           "/output/t_plains2.txt", processor: getRawData )
processFile( "t_forest.png",            "/output/t_forest.txt", processor: getRawData  )
processFile( "t_forest2.png",           "/output/t_forest2.txt", processor: getRawData  )

processFile( "t_mountain.png",          "/output/t_mountain.txt", processor: getRawData  )
processFile( "t_craggy_mountains.png",  "/output/t_craggy_mountains.txt", processor: getRawData )
processFile( "t_downs.png",             "/output/t_downs.txt", processor: getRawData  )
processFile( "t_hills.png",             "/output/t_hills.txt", processor: getRawData  )
processFile( "t_foothills.png",         "/output/t_foothills.txt", processor: getRawData )
processFile( "t_valley.png",            "/output/t_valley.txt", processor: getRawData  )

processFile( "t_bay.png",               "/output/t_bay.txt", processor: getRawData  )
processFile( "t_sea.png",               "/output/t_sea.txt", processor: getRawData  )
processFile( "t_river.png",             "/output/t_river.txt", processor: getRawData  )
processFile( "t_swamp.png",             "/output/t_swamp.txt", processor: getRawData  )
processFile( "t_lake.png",              "/output/t_lake.txt", processor: getRawData  )







