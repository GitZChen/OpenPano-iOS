//
//  PanoViewController.swift
//  OpenPano
//
//  Created by GitZChen on 7/3/16.
//  Copyright © 2019 GitZChen. All rights reserved.
//

import UIKit
import Photos
import Dispatch
import libopano

class PanoViewController: UIViewController {
    
    var pano : UIImage? = nil
    @IBOutlet var imageView: UIImageView!
    
    let stitchingQueue = DispatchQueue.global(qos: .userInitiated)
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
    }
    
    override func viewDidAppear(_ animated: Bool) {
        selectAndStitch()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func selectAndStitch() {
        let welcomeAlertVC = UIAlertController(title: "Welcome!", message: "Please select the images to be stitched.", preferredStyle: .alert)
        welcomeAlertVC.addAction(UIAlertAction(title: "OK", style: .default, handler: { (_) in
            let pickerVC = UIAlertController(style: .actionSheet)
            pickerVC.addPhotoLibraryPicker(flow: .vertical, paging: true, selection: .multiple(action: { (assets) in
                if assets.count < 2 {
                    let fewImageAlertVC = UIAlertController(title: "Too few images!", message: "Please select at least 2 images!", preferredStyle: .alert)
                    fewImageAlertVC.addAction(UIAlertAction(title: "OK", style: .default, handler: { (_) in
                        self.selectAndStitch()
                    }))
                    self.present(fewImageAlertVC, animated: true, completion: nil)
                    return
                }
                
                var imagePaths : [String] = []
                for asset in assets {
                    asset.requestContentEditingInput(with: PHContentEditingInputRequestOptions(), completionHandler: { (input, _) in
                        if input != nil && input?.fullSizeImageURL != nil {
                            if imagePaths.count == 0 {
                                PHImageManager.default().requestImage(for: asset, targetSize: self.view.frame.size, contentMode: .default, options: .init(), resultHandler: { (image, _) in
                                    self.imageView.image = image
                                })
                            }
                            imagePaths.append(input!.fullSizeImageURL!.absoluteString.replacingOccurrences(of: "file://", with: ""))
                        }
                    })
                }
                Timer.scheduledTimer(withTimeInterval: 0.5, repeats: true, block: { (timer) in
                    if assets.count == imagePaths.count {
                        timer.invalidate()
                        if imagePaths.count < 2 {
                            return
                        }
                        self.stitchingQueue.async {
                            self.pano = StitchingWrapper.stitchImages(ofPaths: imagePaths)
                            if self.pano != nil {
                                DispatchQueue.main.async {
                                    self.imageView.image = self.pano
                                }
                            }
                        }
                    }
                })
                
            }))
            self.present(pickerVC, animated: true, completion: nil)
        }))
        
        self.present(welcomeAlertVC, animated: true, completion: nil)
    }
    
}

