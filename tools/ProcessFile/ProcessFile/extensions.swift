//
//  extensions.swift
//  ProcessFile
//
//  Copyright © 2017 Wild, Chris. All rights reserved.
//

import Foundation
import Cocoa

extension NSImage {
    var pngData: Data? {
        guard let tiffRepresentation = tiffRepresentation, let bitmapImage = NSBitmapImageRep(data: tiffRepresentation) else { return nil }
        return bitmapImage.representation(using: .PNG, properties: [:])
    }
    func pngWrite(to url: URL, options: Data.WritingOptions = .atomic) -> Bool {
        do {
            try pngData?.write(to: url, options: options)
            return true
        } catch {
            print(error.localizedDescription)
            return false
        }
    }
}


extension CGColor : Hashable {
    public var hashValue: Int {
        get {
            let parts = self.components!
            return (Int(parts[0]*255)<<24) | (Int(parts[1]*255)<<16) | (Int(parts[2]*255)<<8) | Int(parts[3]*255)
        }
    }
    public var hashValue2: Int {
        get {
            let parts = self.components!
            return (Int(parts[0]*255)<<16) | (Int(parts[1]*255)<<8) | Int(parts[2]*255)
        }
    }
    
}

public func ==(lhs: CGColor, rhs: CGColor) -> Bool
{
    return lhs.hashValue == rhs.hashValue
}
