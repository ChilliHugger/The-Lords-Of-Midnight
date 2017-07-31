//
//  image.swift
//  ProcessFile
//
//  Copyright © 2017 Wild, Chris. All rights reserved.
//

import Foundation
import Cocoa

public struct PixelData {
    var a:UInt8 = 255
    var r:UInt8
    var g:UInt8
    var b:UInt8
}

private let rgbColorSpace = CGColorSpaceCreateDeviceRGB()
private let bitmapInfo:CGBitmapInfo = CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedFirst.rawValue)

public func imageFromARGB32Bitmap(pixels:[PixelData], width:Int, height:Int)->NSImage {
    let bitsPerComponent:Int = 8
    let bitsPerPixel:Int = 32
    
    assert(pixels.count == Int(width * height))
    
    var data = pixels // Copy to mutable []
    let providerRef = CGDataProvider(
        data: NSData(bytes: &data, length: data.count * MemoryLayout<PixelData>.size)
    )
    
    let cgim = CGImage(
        width: width,
        height: height,
        bitsPerComponent: bitsPerComponent,
        bitsPerPixel: bitsPerPixel,
        bytesPerRow: width * Int(MemoryLayout<PixelData>.size),
        space: rgbColorSpace,
        bitmapInfo: bitmapInfo,
        provider: providerRef!,
        decode: nil,
        shouldInterpolate: true,
        intent: CGColorRenderingIntent.defaultIntent
    )
    return NSImage( cgImage: cgim!, size: NSSize(width: width, height: height ) )
}



func getColoursUsed(image1: NSImage) -> [CGColor:Int]
{
    var usedColours = [CGColor:Int]();
    
    guard let image = image1.cgImage(forProposedRect: nil, context: nil, hints: nil) else {
        return usedColours
    }
    
    let pixelsWide = Int(image.width )
    let pixelsHigh = Int(image.height)
    
    let pixelData = image.dataProvider!.data
    let data: UnsafePointer<UInt8> = CFDataGetBytePtr(pixelData)
    
    
    var index = 0
    
    
    for y in 0..<pixelsHigh {
        
        index = 4*((Int(pixelsWide) * Int(y)))
        
        for _ in 0..<pixelsWide {
            
            
            let a = data[index]
            let b = data[index+1]
            let g = data[index+2]
            let r = data[index+3]
            
            index+=4
            
            
            
            let colour = CGColor( red: CGFloat(r)/255.0, green: CGFloat(g)/255.0, blue: CGFloat(b)/255.0, alpha: CGFloat(a)/255.0)
            
            
            if ( usedColours[colour] == nil  ) {
                usedColours[colour] = usedColours.count
            }
            
        }
        
        
    }
    
    
    return usedColours
    
}
